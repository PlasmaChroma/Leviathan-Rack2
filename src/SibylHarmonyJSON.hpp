#pragma once
#include "SibylHarmony.hpp"
#include "SibylJSONUtils.hpp"
#include "SibylNativeHarmony.hpp"
#include <climits>
#include <map>
#include <set>

namespace sibyl { namespace harmony_json {
inline HarmonyBinding binding(json_t *value, bool scene, ParseResult &r, const std::string &path) {
  HarmonyBinding b;
  b.present = value != nullptr;
  if (!value)
    return b;
  if (json_is_null(value)) {
    b.disabled = true;
    return b;
  }
  if (!fields(value, {"progression", "clock", "loop"}, r, path))
    return b;
  b.progression = string(json_object_get(value, "progression"));
  if (!id(b.progression))
    error(r, path + ".progression", "Expected progression ID");
  std::string clock = string(json_object_get(value, "clock"));
  if ((scene && clock != "sceneRepeat" && clock != "sceneVisit") || (!scene && clock != "arrangement"))
    error(r, path + ".clock", "Clock does not match binding scope");
  b.clock = clock == "sceneRepeat"
                ? AutomationClock::SCENE_REPEAT
                : clock == "sceneVisit" ? AutomationClock::SCENE_VISIT : AutomationClock::ARRANGEMENT;
  json_t *loop = json_object_get(value, "loop");
  if (loop && !json_is_boolean(loop))
    error(r, path + ".loop", "Expected boolean");
  b.loop = !loop || json_is_true(loop);
  return b;
}
inline bool expression(json_t *value, HarmonicExpression &e, const Meta &meta, ParseResult &r,
                       const std::string &path) {
  if (!json_is_object(value))
    return error(r, path, "Expected harmonic expression");
  std::string kind = string(json_object_get(value, "kind"));
  e.authored = dump(value);
  if (kind == "tone") {
    if (!fields(value, {"kind", "index", "role", "octave"}, r, path))
      return false;
    json_t *index = json_object_get(value, "index");
    json_t *role = json_object_get(value, "role");
    json_t *octave = json_object_get(value, "octave");
    if (bool(index) == bool(role))
      return error(r, path, "Tone requires exactly one index or role");
    if (index && !integer(index, 0, 7))
      return error(r, path + ".index", "Expected index 0-7");
    if (octave && !integer(octave, -10, 10))
      return error(r, path + ".octave", "Expected octave -10 to 10");
    e.index = int(json_integer_value(index));
    e.octave = int(json_integer_value(octave));
    if (role) {
      std::string name = string(role);
      e.role = name == "root" ? 1 : name == "third" ? 2 : name == "fifth" ? 3 : 0;
      if (!e.role)
        return error(r, path + ".role", "Unknown chord role");
    }
  } else if (kind == "nearest") {
    e.nearest = true;
    if (!fields(value, {"kind", "reference", "range", "tieBreak"}, r, path))
      return false;
    json_t *ref = json_object_get(value, "reference");
    json_t *range = json_object_get(value, "range");
    if (!fields(ref, {"note", "degree", "octave", "pitchV"}, r, path + ".reference") ||
        !fields(range, {"min", "max"}, r, path + ".range"))
      return false;
    json_t *n = json_object_get(ref, "note");
    json_t *d = json_object_get(ref, "degree");
    json_t *v = json_object_get(ref, "pitchV");
    json_t *o = json_object_get(ref, "octave");
    if (bool(n) + bool(d) + bool(v) != 1 || (o && !d))
      return error(r, path + ".reference", "Expected exactly one static pitch; octave requires degree");
    if (n) {
      int pitch = 0;
      if (!note(n, pitch, r, path + ".reference.note"))
        return false;
      e.reference = pitch;
    }
    if (v) {
      if (!number(v, -10, 10))
        return error(r, path + ".reference.pitchV", "Expected -10 to 10 V");
      e.reference = json_number_value(v) * 12.;
    }
    if (d) {
      if (!integer(d, INT32_MIN, INT32_MAX) || (o && !integer(o, INT32_MIN, INT32_MAX)))
        return error(r, path + ".reference", "Invalid degree or octave");
      e.reference = std::round(double(degreeToPitchV(int(json_integer_value(d)), int(json_integer_value(o)), meta.scale,
                                                     meta.root, meta.rootOctave)) *
                               12.);
    }
    if (e.reference < -120.00001 || e.reference > 120.00001)
      return error(r, path + ".reference", "Pitch outside supported domain", "pitch_out_of_range");
    if (!note(json_object_get(range, "min"), e.minimum, r, path + ".range.min") ||
        !note(json_object_get(range, "max"), e.maximum, r, path + ".range.max"))
      return false;
    if (e.minimum > e.maximum)
      return error(r, path + ".range", "Reversed register");
    json_t *tie = json_object_get(value, "tieBreak");
    std::string name = string(tie);
    if (tie && name != "lower" && name != "higher")
      return error(r, path + ".tieBreak", "Expected lower or higher");
    e.higher = name == "higher";
  } else
    return error(r, path + ".kind", "Expected tone or nearest");
  return true;
}
inline bool resolve(const HarmonicExpression &e, const HarmonyChord &chord, float &volts) {
  if (!e.nearest) {
    int index = e.index;
    if (e.role) {
      index = -1;
      for (size_t i = 0; i < chord.intervals.size(); ++i) {
        int pc = chord.intervals[i] % 12;
        bool match = e.role == 1 ? chord.intervals[i] == 0
                                 : e.role == 2 ? (pc == 3 || pc == 4) : (pc == 6 || pc == 7 || pc == 8);
        if (match) {
          if (index >= 0)
            return false;
          index = int(i);
        }
      }
    }
    if (index < 0 || size_t(index) >= chord.intervals.size())
      return false;
    volts = float(chord.rootSemitone + chord.intervals[index] + 12 * e.octave) / 12.f;
    return true;
  }
  bool found = false;
  double best = 1e9;
  int selected = 0;
  for (int pitch = e.minimum; pitch <= e.maximum; ++pitch) {
    bool candidate = false;
    for (int interval : chord.intervals)
      candidate |= ((pitch - chord.rootSemitone - interval) % 12) == 0;
    if (!candidate)
      continue;
    double distance = std::abs(double(pitch) - e.reference);
    if (!found || distance < best || (distance == best && (e.higher ? pitch > selected : pitch < selected))) {
      found = true;
      best = distance;
      selected = pitch;
    }
  }
  volts = selected / 12.f;
  return found;
}
inline json_t *bindingJson(const HarmonyBinding &b) {
  if (b.disabled)
    return json_null();
  return json_pack("{s:s,s:s,s:b}", "progression", b.progression.c_str(), "clock",
                   b.clock == AutomationClock::ARRANGEMENT
                       ? "arrangement"
                       : b.clock == AutomationClock::SCENE_REPEAT ? "sceneRepeat" : "sceneVisit",
                   "loop", b.loop);
}
inline json_t *progressionJson(const HarmonyProgression &p) {
  json_t *out = json_pack("{s:f}", "lengthBeats", p.length);
  if(!p.pitchContext.empty()) json_object_set_new(out,"pitchContext",json_string(p.pitchContext.c_str()));
  json_t *chords = json_array();
  for (const auto &c : p.chords) {
    if(!c.authored.empty()) { json_error_t je; json_array_append_new(chords,json_loads(c.authored.c_str(),0,&je)); continue; }
    json_t *chord = json_pack("{s:s,s:f,s:s}", "id", c.id.c_str(), "beat", c.beat, "root", c.root.c_str());
    json_t *intervals = json_array();
    for (int i : c.intervals)
      json_array_append_new(intervals, json_integer(i));
    json_object_set_new(chord, "intervals", intervals);
    json_array_append_new(chords, chord);
  }
  json_object_set_new(out, "chords", chords);
  return out;
}
} // namespace harmony_json

inline void compileHarmony(json_t *root, Composition &comp, ParseResult &r) {
  using namespace harmony_json;
  json_t *section = json_object_get(root, "harmony");
  comp.harmonyPresent = section != nullptr;
  json_t *definitions = nullptr;
  if (section) {
    if (!fields(section, {"progressions", "default"}, r, "harmony"))
      return;
    definitions = json_object_get(section, "progressions");
    if (definitions && !json_is_object(definitions)) {
      error(r, "harmony.progressions", "Expected map");
      return;
    }
    comp.defaultHarmony = binding(json_object_get(section, "default"), false, r, "harmony.default");
  }
  if (json_object_size(definitions) > 128) {
    error(r, "harmony.progressions", "Maximum 128 progressions", "capacity_exceeded");
    return;
  }
  std::vector<std::string> ids;
  size_t total = 0;
  const char *key;
  json_t *value;
  json_object_foreach(definitions, key, value) {
    ids.emplace_back(key);
    total += json_array_size(json_object_get(value, "chords"));
  }
  if (total > 4096) {
    error(r, "harmony.progressions", "Maximum 4096 chord markers; requested " + std::to_string(total),
          "capacity_exceeded");
    return;
  }
  std::sort(ids.begin(), ids.end());
  std::map<std::vector<int>, int> chordDefinitions;
  int nextChordDefinition=0;
  for (const auto &name : ids) {
    std::string path = "harmony.progressions." + name;
    value = json_object_get(definitions, name.c_str());
    if (!id(name) || !fields(value, {"lengthBeats", "chords", "pitchContext"}, r, path)) {
      error(r, path, "Invalid progression");
      continue;
    }
    HarmonyProgression p;
    p.id = name;
    p.pitchContext=string(json_object_get(value,"pitchContext"));
    if(json_object_get(value,"pitchContext") && !comp.pitchSystems.contexts.count(p.pitchContext)) { error(r,path,"Undefined pitch context","unresolved_pitch_context"); continue; }
    p.length = json_number_value(json_object_get(value, "lengthBeats"));
    if (!number(json_object_get(value, "lengthBeats"), 0, std::numeric_limits<double>::max()) || p.length <= 0) {
      error(r, path + ".lengthBeats", "Expected positive finite length");
      continue;
    }
    json_t *chords = json_object_get(value, "chords");
    if (!json_is_array(chords) || json_array_size(chords) < 1 || json_array_size(chords) > 128) {
      error(r, path + ".chords", "Expected 1-128 markers", "capacity_exceeded");
      continue;
    }
    std::set<std::string> seen;
    size_t i;
    json_t *chord;
    json_array_foreach(chords, i, chord) {
      std::string cp = path + ".chords[" + std::to_string(i) + "]";
      HarmonyChord c;
      if (!fields(chord, {"id", "beat", "root", "intervals", "rootPitch", "tones"}, r, cp))
        continue;
      c.id = string(json_object_get(chord, "id"));
      c.root = string(json_object_get(chord, "root"));
      c.beat = json_number_value(json_object_get(chord, "beat"));
      if (!id(c.id) || !seen.insert(c.id).second) {
        error(r, cp + ".id", "Expected unique chord ID");
        continue;
      }
      if (!number(json_object_get(chord, "beat"), 0, p.length) || c.beat >= p.length ||
          (i == 0 ? c.beat != 0 : (!p.chords.empty() && c.beat <= p.chords.back().beat))) {
        error(r, cp + ".beat", "Expected ordered markers starting at zero, before endpoint");
        continue;
      }
      const bool native=json_object_get(chord,"rootPitch") || json_object_get(chord,"tones");
      if(native != !p.pitchContext.empty() || (native && (json_object_get(chord,"root")||json_object_get(chord,"intervals")))) {
        error(r,cp,"A progression must contain only native chords with pitchContext, or only legacy chords"); continue;
      }
      if(native) {
        if(nativeChord(chord,p.pitchContext,comp,c,r,cp)) { c.definition=nextChordDefinition++;p.chords.push_back(std::move(c)); }
        continue;
      }
      if (!note(json_object_get(chord, "root"), c.rootSemitone, r, cp + ".root"))
        continue;
      json_t *intervals = json_object_get(chord, "intervals");
      if (!json_is_array(intervals) || json_array_size(intervals) < 1 || json_array_size(intervals) > 8) {
        error(r, cp + ".intervals", "Expected 1-8 intervals");
        continue;
      }
      bool valid = true;
      int mask = 0;
      size_t j;
      json_t *interval;
      json_array_foreach(intervals, j, interval) {
        int n = int(json_integer_value(interval));
        if (!integer(interval, 0, 36) || (j == 0 ? n != 0 : n <= c.intervals.back()) || (mask & (1 << (n % 12))) ||
            c.rootSemitone + n > 120) {
          valid = false;
          break;
        }
        mask |= 1 << (n % 12);
        c.intervals.push_back(n);
      }
      if (!valid) {
        error(r, cp + ".intervals",
              "Intervals must be ordered, unique pitch classes, rooted at zero and in pitch bounds");
        continue;
      }
      auto signature = c.intervals;
      signature.insert(signature.begin(), c.rootSemitone);
      auto inserted = chordDefinitions.emplace(signature, nextChordDefinition);
      if(inserted.second) ++nextChordDefinition;
      c.definition = inserted.first->second;
      p.chords.push_back(std::move(c));
    }
    comp.progressions.push_back(std::move(p));
  }
  size_t si;
  json_t *scene;
  json_array_foreach(json_object_get(root, "arrangement"), si, scene) comp.arrangement[si].harmony =
      binding(json_object_get(scene, "harmony"), true, r, "arrangement[" + std::to_string(si) + "].harmony");
  auto bind = [&](HarmonyBinding &b, const std::string &path) {
    if (!b.present || b.disabled)
      return;
    for (size_t i = 0; i < comp.progressions.size(); ++i)
      if (comp.progressions[i].id == b.progression)
        b.compiledProgression = int(i);
    if (b.compiledProgression < 0)
      error(r, path, "Undefined progression", "unresolved_harmony");
  };
  bind(comp.defaultHarmony, "harmony.default");
  for (auto &s : comp.arrangement)
    bind(s.harmony, "arrangement." + s.id + ".harmony");
  std::map<std::string, int> expressions;
  std::vector<std::string> patterns;
  for (const auto &p : comp.patterns)
    patterns.push_back(p.first);
  std::sort(patterns.begin(), patterns.end());
  for (const auto &name : patterns)
    for (auto &event : comp.patterns[name].steps)
      if (event.pitchType == PitchType::HARMONIC) {
        const std::string expressionKey=event.harmonic+"\n"+comp.patterns[name].pitchContext;
        auto found = expressions.find(expressionKey);
        if (found != expressions.end()) {
          event.harmonicExpression = found->second;
          continue;
        }
        json_error_t je;
        json_t *authored = json_loads(event.harmonic.c_str(), 0, &je);
        HarmonicExpression e;
        json_t* ref=json_object_get(authored,"reference"), *range=json_object_get(authored,"range");
        const auto role=string(json_object_get(authored,"role"));
        bool extended=json_object_get(authored,"periods") || json_object_get(authored,"toneId") || json_object_get(ref,"tuned") ||
          json_is_object(json_object_get(range,"min")) || json_is_object(json_object_get(range,"max")) ||
          (!role.empty() && role!="root" && role!="third" && role!="fifth");
        const auto path="patterns."+name+".notes."+event.id+".harmonic";
        bool valid=extended ? nativeExpression(authored,e,comp,comp.patterns[name].pitchContext,r,path) : expression(authored,e,comp.meta,r,path);
        json_decref(authored);
        if (valid) {
          event.harmonicExpression = int(comp.harmonicExpressions.size());
          expressions[expressionKey] = event.harmonicExpression;
          comp.harmonicExpressions.push_back(std::move(e));
        }
      }
  if (!r.errors.empty())
    return;
  bool any = !comp.progressions.empty() || !comp.harmonicExpressions.empty();
  if (!any)
    return;
  // Automation and harmony share exactly the same score prefix convention.
  if (comp.sceneBeatPrefixes.empty()) {
    for (const auto &s : comp.arrangement) {
      comp.sceneBeatPrefixes.push_back(comp.arrangementDuration);
      double end = comp.arrangementDuration + sceneTimelineLength(s) * s.repeats;
      if (!std::isfinite(end) || end <= comp.arrangementDuration) {
        error(r, "harmony", "Unrepresentable score interval", "capacity_exceeded");
        return;
      }
      comp.arrangementDuration = end;
    }
  }
  std::set<std::string> assigned;
  std::vector<std::map<int, float>> tables(comp.harmonicExpressions.size());
  size_t storage = comp.pitchStorageBytes + comp.progressions.capacity() * sizeof(HarmonyProgression) +
                   comp.harmonicExpressions.capacity() * sizeof(HarmonicExpression);
  for (const auto &progression : comp.progressions) {
    storage += progression.id.capacity() + progression.pitchContext.capacity() + 2 + progression.chords.capacity() * sizeof(HarmonyChord);
    for (const auto &chord : progression.chords)
      {
      storage += chord.id.capacity() + chord.root.capacity() + chord.context.capacity()+chord.authored.capacity()+4 + chord.intervals.capacity() * sizeof(int)+chord.tones.capacity()*sizeof(NativeChordTone);
      for(const auto& tone:chord.tones) { storage+=tone.id.capacity()+tone.pitch.context.capacity()+2+tone.roles.capacity()*sizeof(std::string); for(const auto& role:tone.roles) storage+=role.capacity()+1; }
    }
  }
  for (const auto &curve : comp.automation)
    storage += sizeof(AutomationCurve) + 192 + curve.points.size() * sizeof(AutomationPoint) +
               curve.segments.size() * sizeof(AutomationSegment);
  storage += comp.arrangement.size() * (sizeof(AutomationRoute) + sizeof(double));
  for (const auto &e : comp.harmonicExpressions)
    storage += e.authored.capacity() + e.roleName.capacity() + e.toneId.capacity() + 3;
  for (const auto &pattern : comp.patterns)
    for (const auto &event : pattern.second.steps)
      if (event.pitchType == PitchType::HARMONIC)
        storage += event.harmonic.capacity() + 1;
  if (storage > 32u * 1024u * 1024u) {
    error(r, "harmony", "Compiled storage exceeds 32 MiB", "capacity_exceeded");
    return;
  }
  std::map<std::string,std::shared_ptr<const std::vector<HarmonicRoutePitch>>> routePrograms;
  for (size_t s = 0; s < comp.arrangement.size(); ++s) {
    auto &sc = comp.arrangement[s];
    const auto *b = effectiveHarmony(comp, s);
    std::vector<size_t> reachable;
    if (b) {
      const auto &p = comp.progressions[b->compiledProgression];
      double start = b->clock == AutomationClock::ARRANGEMENT ? comp.sceneBeatPrefixes[s] : 0.;
      double duration = sceneTimelineLength(sc) * (b->clock == AutomationClock::SCENE_REPEAT ? 1 : sc.repeats);
      if (b->loop &&
          (!std::isfinite((start + duration) / p.length) || (start + duration) / p.length > 4503599627370495.)) {
        error(r, "arrangement." + sc.id, "Progression cycles exceed coordinate precision", "capacity_exceeded");
        return;
      }
      for (size_t c = 0; c < p.chords.size(); ++c) {
        double begin = p.chords[c].beat, end = c + 1 < p.chords.size() ? p.chords[c + 1].beat : p.length;
        bool reaches = false;
        if (b->loop) {
          double offset = start - std::floor(start / p.length) * p.length;
          reaches = duration >= p.length || (begin < offset + duration && end > offset) ||
                    (begin < offset + duration - p.length);
        } else
          reaches = begin < start + duration && (c + 1 == p.chords.size() || end > start);
        if (reaches)
          reachable.push_back(c);
        if (start + begin == start + end) {
          error(r, "arrangement." + sc.id, "Chord interval collapses at score precision", "capacity_exceeded");
          return;
        }
      }
    }
    for (auto &assignment : sc.tracks) {
      std::shared_ptr<std::vector<HarmonicRoutePitch>> route(new std::vector<HarmonicRoutePitch>);
      auto pat = comp.patterns.find(assignment.second.patternId);
      if (pat == comp.patterns.end())
        continue;
      assigned.insert(pat->first);
      std::string routeKey=pat->first;routeKey.push_back(0);
      auto appendKey=[&](const void* data,size_t bytes){routeKey.append(static_cast<const char*>(data),bytes);};
      const auto& offsets=assignment.second.overrides.pitchOffsets;
      appendKey(&offsets.fields,sizeof(offsets.fields));appendKey(&offsets.steps,sizeof(offsets.steps));appendKey(&offsets.periods,sizeof(offsets.periods));appendKey(&offsets.cents,sizeof(offsets.cents));
      appendKey(&assignment.second.overrides.values[TRANSPOSE],sizeof(float));
      if(b) for(size_t ci:reachable) {int definition=comp.progressions[b->compiledProgression].chords[ci].definition;appendKey(&definition,sizeof(definition));}
      auto cached=routePrograms.find(routeKey);
      if(cached!=routePrograms.end())assignment.second.harmonicPitches=cached->second;
      for (const auto &event : pat->second.steps)
        if (event.pitchType == PitchType::HARMONIC) {
          std::string path = "arrangement." + sc.id + ".tracks." + assignment.first + ".notes." + event.id;
          if (!b) {
            error(r, path, "Assigned harmonic event has no effective harmony", "unresolved_harmony");
            continue;
          }
          auto &table = tables[event.harmonicExpression];
          const auto &p = comp.progressions[b->compiledProgression];
          for (size_t ci : reachable) {
            const auto &chord = p.chords[ci];
            if(!chord.tones.empty() || comp.harmonicExpressions[event.harmonicExpression].extended) {
              if(cached!=routePrograms.end())continue;
              HarmonyChord converted;
              const HarmonyChord* nativeChord=&chord;
              if(chord.tones.empty()) {
                if(!comp.harmonicExpressions[event.harmonicExpression].toneId.empty()) {error(r,path,"toneId requires native harmony","unresolved_harmony");continue;}
                converted.periodV=1.;
                for(size_t i=0;i<chord.intervals.size();++i) {
                  NativeChordTone tone;tone.id=std::to_string(i);tone.pitch.baseV=(chord.rootSemitone+chord.intervals[i])/12.;
                  for(int role=1;role<=3;++role) {HarmonicExpression probe;probe.role=role;float v=0.;if(resolve(probe,chord,v)&&std::abs(double(v)-tone.pitch.baseV)<1e-6)tone.roles.push_back(role==1?"root":role==2?"third":"fifth");}
                  converted.tones.push_back(std::move(tone));
                }
                nativeChord=&converted;
              }
              const size_t needed=table.count(chord.definition)?1u:2u;
              if(comp.harmonicPitchEntries+comp.staticPitchEntries>1048576-needed || storage+sizeof(HarmonicRoutePitch)*2+(needed-1)*sizeof(HarmonicExpression::Pitch)>32u*1024u*1024u) { error(r,path,"Native harmonic pitch capacity exceeded","capacity_exceeded"); return; }
              NativePitch selected;
              if(!resolveNative(comp.harmonicExpressions[event.harmonicExpression],*nativeChord,comp.pitchSystems,selected)) { error(r,path,"Cannot resolve native tone against chord "+chord.id,"unresolved_harmony"); continue; }
              StepEvent transformedEvent=event; transformedEvent.nativePitch=selected;
              double voltage=0.;
              if(!tuning_json::transformed(transformedEvent,comp.pitchSystems,&assignment.second.overrides,voltage,r,path)) continue;
              route->push_back({event.step,chord.definition,float(voltage)});
              if(!table.count(chord.definition)) { table[chord.definition]=float(selected.baseV); ++comp.harmonicPitchEntries; storage+=sizeof(HarmonicExpression::Pitch); }
              ++comp.harmonicPitchEntries; storage+=sizeof(HarmonicRoutePitch)*2;
              continue;
            }
            if((event.pitchOffsets.fields|assignment.second.overrides.pitchOffsets.fields)&1) { error(r,path,"Legacy harmony has no native lattice","unsupported_pitch_transform"); continue; }
            if(comp.harmonicExpressions[event.harmonicExpression].extended) { error(r,path,"Native harmonic expression requires native harmony","unresolved_harmony"); continue; }
            auto found = table.find(chord.definition);
            float pitch = 0.;
            if (found == table.end()) {
              if (comp.harmonicPitchEntries + comp.staticPitchEntries >= 1048576 ||
                  storage + sizeof(HarmonicExpression::Pitch) > 32u * 1024u * 1024u) {
                error(r, path, "Pitch table capacity exceeded; entries=" + std::to_string(comp.harmonicPitchEntries),
                      "capacity_exceeded");
                return;
              }
              if (!resolve(comp.harmonicExpressions[event.harmonicExpression], chord, pitch)) {
                error(r, path, "Cannot resolve expression against chord " + chord.id, "unresolved_harmony");
                continue;
              }
              table[chord.definition] = pitch;
              ++comp.harmonicPitchEntries;
              storage += sizeof(HarmonicExpression::Pitch);
            } else
              pitch = found->second;
            float final = assignedPitch(event,assignment.second,event.pitchOffsets.fields ? float(double(pitch)+double(event.transposeSemitones)/12.+event.pitchOffsets.periods+event.pitchOffsets.cents/1200.) : pitch + event.transposeSemitones / 12.f);
            const auto& offset=assignment.second.overrides.pitchOffsets;
            const bool nativeOffsets=event.pitchOffsets.fields || offset.fields;
            const double checkedFinal=nativeOffsets ? double(pitch)+double(event.transposeSemitones)/12.+event.pitchOffsets.periods+event.pitchOffsets.cents/1200.+double(assignment.second.overrides.values[TRANSPOSE])/12.+offset.periods+offset.cents/1200. : double(final);
            if (!pitchInDomain(checkedFinal))
              error(r, path, "Resolved/transposed pitch outside -10 to 10 V", "pitch_out_of_range");
          }
        }
      if(!route->empty()) {
        std::sort(route->begin(),route->end(),[](const HarmonicRoutePitch& a,const HarmonicRoutePitch& b) { return a.step==b.step?a.chord<b.chord:a.step<b.step; });
        assignment.second.harmonicPitches=route;routePrograms.emplace(std::move(routeKey),route);
      }
    }
  }
  if (storage > 32u * 1024u * 1024u) {
    error(r, "harmony", "Compiled storage exceeds 32 MiB", "capacity_exceeded");
    return;
  }
  for (size_t i = 0; i < tables.size(); ++i) {
    comp.harmonicExpressions[i].pitches.reserve(tables[i].size());
    for (const auto &pitch : tables[i])
      comp.harmonicExpressions[i].pitches.push_back({pitch.first, pitch.second});
  }
  for (const auto &name : patterns)
    if (!assigned.count(name))
      for (const auto &event : comp.patterns[name].steps)
        if (event.pitchType == PitchType::HARMONIC) {
          r.warnings.push_back({"patterns." + name, "Harmonic pattern is not assigned", "unbound_harmony_pattern"});
          break;
        }
}
} // namespace sibyl
