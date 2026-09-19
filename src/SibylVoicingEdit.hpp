#pragma once
#include "SibylEdit.hpp"
#include "SibylHarmonyJSON.hpp"
#include "SibylVoicing.hpp"
#include <set>

namespace sibyl {
inline bool applyVoicingOperation(json_t *working, json_t *op, size_t operationIndex, EditResult &result) {
  using namespace harmony_json;
  const std::string path = "operations[" + std::to_string(operationIndex) + "]";
  auto fail = [&](const std::string &code, const std::string &field, const std::string &message) {
    result.errorCode = code;
    result.errorPath = path + field;
    result.errorMessage = message;
    return false;
  };
  ParseResult validation;
  if (!fields(op, {"op", "progression_id", "scene_id", "resolution", "gate_ratio", "write_policy", "voices"},
              validation, path))
    return fail("invalid_operation", "", validation.errors.front().message);
  const std::string progressionId = string(json_object_get(op, "progression_id")),
                    sceneId = string(json_object_get(op, "scene_id"));
  const std::string resolution = string(json_object_get(op, "resolution"));
  if (!id(progressionId) || !id(sceneId) || resolution.empty())
    return fail("invalid_operation", "", "Expected progression_id, scene_id and resolution");
  json_t *progression =
      json_object_get(json_object_get(json_object_get(working, "harmony"), "progressions"), progressionId.c_str());
  if (!progression)
    return fail("object_not_found", ".progression_id", "Progression not found");
  json_t *scene = nullptr;
  size_t i;
  json_t *value;
  json_array_foreach(json_object_get(working, "arrangement"), i,
                     value) if (string(json_object_get(value, "id")) == sceneId) scene = value;
  if (!scene)
    return fail("object_not_found", ".scene_id", "Scene not found");
  json_t *ratioJ = json_object_get(op, "gate_ratio");
  double ratio = ratioJ ? json_number_value(ratioJ) : .95;
  if (ratioJ && !number(ratioJ, 0, 1))
    return fail("invalid_operation", ".gate_ratio", "Expected finite gate ratio 0-1");
  json_t *policyJ = json_object_get(op, "write_policy");
  std::string policy = policyJ ? string(policyJ) : "createOnly";
  if (policy != "createOnly" && policy != "replace")
    return fail("invalid_operation", ".write_policy", "Expected createOnly or replace");
  json_t *list = json_object_get(op, "voices");
  if (!json_is_array(list) || json_array_size(list) < 1 || json_array_size(list) > 8)
    return fail("invalid_operation", ".voices", "Expected 1-8 voices in low-to-high order");
  // Compile only the requested progression and grid here. Other operations in
  // this transaction may temporarily detach/repair unrelated score references.
  json_t *isolated =
      json_pack("{s:i,s:{s:{s:O}},s:{s:{s:i,s:s}}}", "schemaVersion", 4, "harmony", "progressions",
                progressionId.c_str(), progression, "patterns", "grid", "length", 1, "resolution", resolution.c_str());
  if(json_object_get(progression,"pitchContext")) {
    json_t* all=json_object_get(working,"pitchSystems");json_t* subset=json_pack("{s:{},s:{},s:{}}","tunings","scales","contexts");
    std::set<std::string> contextIds{string(json_object_get(progression,"pitchContext"))};
    size_t vi;json_t* voice;json_array_foreach(list,vi,voice) for(const char* endpoint:{"min","max"}) {
      json_t* context=json_object_get(json_object_get(json_object_get(voice,endpoint),"tuned"),"context");if(context)contextIds.insert(string(context));
    }
    for(const auto& contextId:contextIds) {
      json_t* context=json_object_get(json_object_get(all,"contexts"),contextId.c_str());
      if(!context)continue;
      json_object_set(json_object_get(subset,"contexts"),contextId.c_str(),context);
      auto tuningId=string(json_object_get(context,"tuning")),scaleId=string(json_object_get(context,"scale"));
      json_object_set(json_object_get(subset,"tunings"),tuningId.c_str(),json_object_get(json_object_get(all,"tunings"),tuningId.c_str()));
      if(!scaleId.empty())json_object_set(json_object_get(subset,"scales"),scaleId.c_str(),json_object_get(json_object_get(all,"scales"),scaleId.c_str()));
    }
    json_object_set_new(isolated,"pitchSystems",subset);
  }
  auto compiled = parseCompositionJson(dump(isolated), 0);
  json_decref(isolated);
  if (!compiled.valid) {
    if (compiled.errors.empty())
      return fail("invalid_operation", ".progression_id", "Invalid progression/grid");
    const auto &issue = compiled.errors.front();
    return fail(issue.code.empty() ? "invalid_operation" : issue.code,
                issue.path.find("patterns.grid") == 0 ? ".resolution" : ".progression_id", issue.message);
  }
  const auto &source = compiled.composition->progressions.front();
  const bool native=!source.pitchContext.empty();
  std::vector<VoicingVoice> voices;
  std::set<std::string> tracks, patterns;
  json_t *bank = json_object_get(working, "patterns");
  json_t *assignments = json_object_get(scene, "tracks");
  json_array_foreach(list, i, value) {
    std::string vp = path + ".voices[" + std::to_string(i) + "]";
    VoicingVoice voice;
    if (!fields(value, {"track_id", "pattern_id", "min", "max"}, validation, vp))
      return fail("invalid_operation", ".voices", validation.errors.back().message);
    voice.track = string(json_object_get(value, "track_id"));
    voice.pattern = string(json_object_get(value, "pattern_id"));
    if (!id(voice.track) || !id(voice.pattern) || !tracks.insert(voice.track).second ||
        !patterns.insert(voice.pattern).second)
      return fail("invalid_operation", ".voices", "Track and pattern IDs must be valid and distinct");
    bool exists = false;
    size_t t;
    json_t *track;
    json_array_foreach(json_object_get(working, "tracks"), t, track) exists |=
        string(json_object_get(track, "id")) == voice.track;
    if (!exists)
      return fail("object_not_found", ".voices", "Target track does not exist: " + voice.track);
    if(native) {
      NativePitch lo,hi;
      if(!staticPitch(json_object_get(value,"min"),source.pitchContext,*compiled.composition,lo,validation,vp+".min",true) ||
         !staticPitch(json_object_get(value,"max"),source.pitchContext,*compiled.composition,hi,validation,vp+".max",true) || lo.baseV>hi.baseV)
        return fail("invalid_operation",".voices","Expected ordered precise static register");
      voice.minimumV=lo.baseV;voice.maximumV=hi.baseV;voice.minimum=voicingTick(lo.baseV);voice.maximum=voicingTick(hi.baseV);
    } else if (!note(json_object_get(value, "min"), voice.minimum, validation, vp + ".min") ||
        !note(json_object_get(value, "max"), voice.maximum, validation, vp + ".max") || voice.minimum > voice.maximum)
      return fail("invalid_operation", ".voices", "Expected ordered inclusive scientific-note register");
    if (policy == "createOnly" &&
        (json_object_get(bank, voice.pattern.c_str()) || json_object_get(assignments, voice.track.c_str())))
      return fail("object_in_use", ".voices", "createOnly requires unused patterns and unassigned destination tracks");
    voices.push_back(std::move(voice));
  }
  double grid = compiled.composition->patterns.at("grid").resolutionBeats;
  json_t *sceneLengthJ = json_object_get(scene, "lengthBeats");
  double sceneLength = sceneLengthJ ? json_number_value(sceneLengthJ) : 16.;
  if (sceneLength != source.length)
    return fail("off_grid_harmony", ".scene_id", "Scene length must equal progression length");
  auto onGrid = [&](double beat) {
    double step = std::round(beat / grid);
    return std::isfinite(step) && step >= 0 && step <= 1024 && step * grid == beat;
  };
  if (!onGrid(source.length) || source.length / grid < 1)
    return fail("off_grid_harmony", ".resolution", "Progression must occupy 1-1024 exact grid steps");
  for (const auto &chord : source.chords)
    if (!onGrid(chord.beat))
      return fail("off_grid_harmony", ".resolution", "Chord marker is off grid: " + chord.id);
  VoicingBudget budget;
  budget.partials = result.voicingPartials;
  budget.transitions = result.voicingTransitions;
  auto solution = native ? voiceNativeProgression(source,voices,budget) : voiceProgression(source, voices, budget);
  result.voicingPartials = budget.partials;
  result.voicingTransitions = budget.transitions;
  if (!solution.valid)
    return fail(solution.code, ".voices",
                solution.message + "; partials=" + std::to_string(budget.partials) +
                    ", transitions=" + std::to_string(budget.transitions));

  json_t *report =
      json_pack("{s:i,s:s,s:s,s:s,s:s,s:b,s:b,s:i,s:i}", "operationIndex", int(operationIndex), "operation",
                "voice_progression", "algorithm", native ? "voice_progression_micro_v1" : "voice_progression_v1", "progressionId", progressionId.c_str(),
                "sceneId", sceneId.c_str(), "materializesNotes", 1, "optimizesLoopSeam", 0, "visitedPartials",
                int(budget.partials), "evaluatedTransitions", int(budget.transitions));
  json_t *references = json_array();
  size_t referencesTotal = 0;
  size_t si;
  json_t *sc;
  json_array_foreach(json_object_get(working, "arrangement"), si, sc) {
    const char *trackId;
    json_t *assignment;
    json_object_foreach(json_object_get(sc, "tracks"), trackId, assignment) {
      std::string p = json_is_string(assignment) ? string(assignment) : string(json_object_get(assignment, "pattern"));
      if (patterns.count(p) || (sc == scene && tracks.count(trackId))) {
        ++referencesTotal;
        if (json_array_size(references) < 128)
          json_array_append_new(references,
                                json_pack("{s:s,s:s,s:s}", "sceneId", string(json_object_get(sc, "id")).c_str(),
                                          "trackId", trackId, "patternId", p.c_str()));
      }
    }
  }
  json_object_set_new(report, "affectedReferences", references);
  json_object_set_new(report, "affectedReferencesTotal", json_integer(referencesTotal));
  json_object_set_new(report, "referencesTruncated", json_boolean(referencesTotal > 128));
  if(!native) json_object_set_new(report, "objective",
                      json_pack("{s:i,s:i,s:I,s:I,s:f}", "missingPitchClasses", solution.missing, "maximumLeap",
                                solution.maximumLeap, "totalMovement", json_int_t(solution.movement), "squaredMovement",
                                json_int_t(solution.squaredMovement), "initialDisplacement",
                                solution.initialDisplacementTwice * .5));
  if(native) {
    json_object_set_new(report,"objective",json_pack("{s:i,s:i,s:I,s:I,s:f}","missingTones",solution.missing,"maximumLeapMilliCents",solution.maximumLeap,"totalMovementMilliCents",json_int_t(solution.movement),"squaredMovementMilliCents2",json_int_t(solution.squaredMovement),"initialDisplacementMilliCents",solution.initialDisplacementTwice*.5));
    json_object_set_new(report,"scoringResolutionCents",json_real(.001));
    json_object_set_new(report,"pitchContext",json_string(source.pitchContext.c_str()));
    json_object_set_new(report,"tuning",json_string(compiled.composition->pitchSystems.contexts.at(source.pitchContext).tuning.c_str()));
    json_t* provenance=json_array();
    for(size_t c=0;c<source.chords.size();++c) for(size_t v=0;v<voices.size();++v) {
      const auto& chosen=solution.selected[c];
      json_array_append_new(provenance,json_pack("{s:s,s:s,s:s,s:I,s:f,s:f}","chordId",source.chords[c].id.c_str(),"trackId",voices[v].track.c_str(),"toneId",source.chords[c].tones[chosen.toneIndices[v]].id.c_str(),"periods",json_int_t(chosen.periods[v]),"pitchV",chosen.volts[v],"cents",chosen.volts[v]*1200.));
    }
    json_object_set_new(report,"provenance",provenance);
  }
  json_t *destinations = json_array();
  if (!assignments) {
    assignments = json_object();
    json_object_set_new(scene, "tracks", assignments);
  }
  for (size_t v = 0; v < voices.size(); ++v) {
    const auto &voice = voices[v];
    json_t *steps = json_array();
    for (size_t chord = 0; chord < source.chords.size(); ++chord) {
      double end = chord + 1 < source.chords.size() ? source.chords[chord + 1].beat : source.length;
      double gate = ((end - source.chords[chord].beat) / grid) * ratio;
      json_array_append_new(steps,
                            json_pack("{s:i,s:f,s:f,s:i}", "step", int(std::round(source.chords[chord].beat / grid)),
                                      "pitchV", native ? solution.selected[chord].volts[v] : solution.pitches[chord][v] / 12., "gate", gate, "ratchets", 1));
    }
    json_t *pattern = json_pack("{s:i,s:s,s:o}", "length", int(std::round(source.length / grid)), "resolution",
                                resolution.c_str(), "steps", steps);
    ParseResult normalized;
    if (!normalizeNoteIds(pattern, "patterns." + voice.pattern, normalized,
                          json_object_get(bank, voice.pattern.c_str()))) {
      json_decref(pattern);
      json_decref(destinations);
      json_decref(report);
      return fail(normalized.errors.front().code, ".voices", normalized.errors.front().message);
    }
    json_object_set_new(bank, voice.pattern.c_str(), pattern);
    json_object_set_new(assignments, voice.track.c_str(),
                        json_pack("{s:s,s:s}", "pattern", voice.pattern.c_str(), "phaseMode", "restart"));
    json_array_append_new(destinations, json_pack("{s:s,s:s,s:i}", "trackId", voice.track.c_str(), "patternId",
                                                  voice.pattern.c_str(), "inserted", int(source.chords.size())));
  }
  json_object_set_new(report, "voices", destinations);
  result.voicingChanges.push_back(dump(report));
  json_decref(report);
  return true;
}
} // namespace sibyl
