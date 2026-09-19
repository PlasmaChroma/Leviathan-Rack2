#pragma once
#include "SibylHarmonyJSON.hpp"
namespace sibyl {
inline json_t *chordContextJson(const HarmonyProgression &p, size_t index) {
  const auto &c = p.chords[index];
  json_t *out = json_pack("{s:s,s:f,s:f,s:s}", "id", c.id.c_str(), "beginBeat", c.beat, "endBeat",
                          index + 1 < p.chords.size() ? p.chords[index + 1].beat : p.length, "root", c.root.c_str());
  if(!c.tones.empty()) {
    json_object_del(out,"root");json_object_set_new(out,"pitchContext",json_string(c.context.c_str()));json_object_set_new(out,"periodCents",json_real(c.periodV*1200.));
    json_t* tones=json_array();json_t* roles=json_object();
    for(const auto& tone:c.tones) { json_array_append_new(tones,json_pack("{s:s,s:f}","id",tone.id.c_str(),"pitchV",tone.pitch.baseV)); for(const auto& role:tone.roles) json_object_set_new(roles,role.c_str(),json_real(tone.pitch.baseV)); }
    json_object_set_new(out,"tones",tones);json_object_set_new(out,"rolesPitchV",roles);return out;
  }
  json_t *pcs = json_array();
  json_t *roles = json_object();
  for (int interval : c.intervals)
    json_array_append_new(pcs, json_integer(((c.rootSemitone + interval) % 12 + 12) % 12));
  for (int role = 1; role <= 3; ++role) {
    HarmonicExpression expression;
    expression.role = role;
    float pitch;
    json_object_set_new(roles, role == 1 ? "root" : role == 2 ? "third" : "fifth",
                        harmony_json::resolve(expression, c, pitch) ? json_real(pitch) : json_null());
  }
  json_object_set_new(out, "pitchClasses", pcs);
  json_object_set_new(out, "rolesPitchV", roles);
  return out;
}
inline std::string serializeHarmonyView(const Composition &comp, json_t *request) {
  using namespace harmony_json;
  auto fail = [](const char *message) {
    json_t *out = json_pack("{s:b,s:{s:s,s:s}}", "ok", 0, "error", "code", "invalid_request", "message", message);
    auto text = dump(out);
    json_decref(out);
    return text;
  };
  std::string view = string(json_object_get(request, "view"));
  json_t *out = nullptr;
  if (view == "progression") {
    std::string idValue = string(json_object_get(request, "id"));
    const HarmonyProgression *p = nullptr;
    for (const auto &item : comp.progressions)
      if (item.id == idValue)
        p = &item;
    if (!p)
      return fail("Progression not found; id required");
    out =
        json_pack("{s:b,s:i,s:s,s:s}", "ok", 1, "revision", comp.revision, "view", "progression", "id", p->id.c_str());
    json_object_set_new(out, "progression", progressionJson(*p));
    json_t *chords = json_array();
    for (size_t i = 0; i < p->chords.size(); ++i)
      json_array_append_new(chords, chordContextJson(*p, i));
    json_object_set_new(out, "derived", json_pack("{s:o}", "chords", chords));
  } else {
    std::string idValue = string(json_object_get(request, "scene_id"));
    size_t s = 0;
    for (; s < comp.arrangement.size(); ++s)
      if (comp.arrangement[s].id == idValue)
        break;
    if (s == comp.arrangement.size())
      return fail("scene_id must identify an authored scene");
    const auto &scene = comp.arrangement[s];
    json_t *r = json_object_get(request, "scene_repeat");
    json_t *t = json_object_get(request, "beat");
    if (!integer(r, 0, scene.repeats - 1) || !number(t, 0, sceneTimelineLength(scene)) ||
        json_number_value(t) >= sceneTimelineLength(scene))
      return fail("Expected authored scene_repeat and beat in half-open repeat interval");
    int repeat = int(json_integer_value(r));
    double beat = json_number_value(t);
    out = json_pack("{s:b,s:i,s:s,s:s,s:i,s:f}", "ok", 1, "revision", comp.revision, "view", "effective_context",
                    "scene_id", idValue.c_str(), "scene_repeat", repeat, "beat", beat);
    const auto *b = effectiveHarmony(comp, s);
    json_object_set_new(out, "binding", b ? bindingJson(*b) : json_null());
    json_t *derived = json_object();
    json_object_set_new(out, "derived", derived);
    if (b) {
      const auto &p = comp.progressions[b->compiledProgression];
      double coordinate = harmonyCoordinate(comp, s, repeat, beat, *b);
      json_object_set_new(derived, "coordinate", json_real(coordinate));
      json_object_set_new(derived, "chord", chordContextJson(p, harmonyChordIndex(p, coordinate, b->loop)));
    } else
      json_object_set_new(derived, "chord", json_null());
    json_t *notes = json_array();
    size_t total = 0;
    for (const auto &track : comp.tracks) {
      auto assignment = scene.tracks.find(track.id);
      if (assignment == scene.tracks.end())
        continue;
      auto pattern = comp.patterns.find(assignment->second.patternId);
      if (pattern == comp.patterns.end())
        continue;
      for (const auto &event : pattern->second.steps) {
        ++total;
        if (json_array_size(notes) >= 256)
          continue;
        float selected = event.pitchType == PitchType::HARMONIC
                             ? harmonicSelectedPitch(comp, event, s, repeat, beat)
                             : float(event.nativePitch.baseV);
        json_t *projection = json_pack("{s:s,s:s,s:i,s:f,s:f}", "track", track.id.c_str(), "noteId", event.id.c_str(),
                                       "step", event.step, "selectedPitchV", double(selected), "effectivePitchV",
                                       double(sceneEventPitch(comp,event,assignment->second,s,repeat,beat)));
        const auto &ov = assignment->second.overrides;
        auto unit = [](float v) { return std::max(0.f, std::min(1.f, v)); };
        json_object_set_new(projection, "velocity",
                            json_real(unit(ov.scaled(event.hasVelocity ? event.velocity : track.defaultVelocity,
                                                     VELOCITY_SCALE, VELOCITY_OFFSET))));
        json_object_set_new(projection, "probability",
                            json_real(unit(ov.scaled(event.hasProbability ? event.probability : 1.f, PROBABILITY_SCALE,
                                                     PROBABILITY_OFFSET))));
        float gate = ov.scaled(event.hasGate ? event.gate : track.defaultGate, GATE_SCALE, GATE_OFFSET);
        json_object_set_new(projection, "gate",
                            json_real(ov.gateTransform() && gate <= 0
                                          ? 0
                                          : std::max(.01f, std::min(event.ratchets > 1 ? 1.f : 1024.f, gate))));
        json_array_append_new(notes, projection);
      }
    }
    json_object_set_new(derived, "notes", notes);
    json_object_set_new(derived, "totalNotes", json_integer(total));
    json_object_set_new(derived, "truncated", json_boolean(total > 256));
    json_object_set_new(derived, "expressionContext", json_string("authoredBeforeEvolutionAndMacros"));
    json_t *automation = json_array();
    if (s < comp.automationRoutes.size())
      for (const auto &track : comp.tracks)
        for (int lane = 0; lane < 3; ++lane) {
          int owner = comp.automationRoutes[s][track.channel * 3 + lane];
          if (owner < 0)
            continue;
          const auto &curve = comp.automation[owner];
          size_t cursor = 0;
          double visit = repeat * sceneTimelineLength(scene) + beat;
          double coordinate = curve.clock == AutomationClock::ARRANGEMENT
                                  ? comp.sceneBeatPrefixes[s] + visit
                                  : curve.clock == AutomationClock::SCENE_VISIT ? visit : beat;
          double value = sampleAutomation(curve, coordinate, cursor);
          auto assignment = scene.tracks.find(track.id);
          double offset = assignment == scene.tracks.end() ? 0 : assignment->second.overrides.values[MOD_OFFSET + lane];
          json_array_append_new(automation, json_pack("{s:s,s:s,s:s,s:f,s:f,s:s}", "track", track.id.c_str(), "lane",
                                                      lane == 0 ? "mod" : lane == 1 ? "mod2" : "mod3", "id",
                                                      curve.id.c_str(), "curveValue", value, "curvePlusSceneOffset",
                                                      value + offset, "mode", curve.add ? "add" : "replace"));
        }
    json_object_set_new(derived, "automation", automation);
  }
  json_object_set_new(out, "schemaVersion", json_integer(4));
  std::string result = dump(out);
  json_decref(out);
  return result;
}
} // namespace sibyl
