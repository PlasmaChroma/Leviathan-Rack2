#pragma once
#include "SibylHarmony.hpp"
#include "SibylJSON.hpp"
#include <climits>
#include <map>
#include <set>

namespace sibyl {
float degreeToPitchV(int, int, ScaleType, const std::string &, int);
float noteToPitchV(const std::string &, ParseResult &, const std::string &);
namespace harmony_json {
inline std::string dump(json_t *value) {
  char *text = json_dumps(value, JSON_COMPACT | JSON_SORT_KEYS | JSON_ENCODE_ANY);
  std::string result = text ? text : "null";
  free(text);
  return result;
}
inline std::string string(json_t *value) {
  return json_is_string(value) ? std::string(json_string_value(value), json_string_length(value)) : std::string();
}
inline bool error(ParseResult &result, const std::string &path, const std::string &message,
                  const char *code = "invalid_harmony") {
  result.errors.push_back({path, message, code});
  result.valid = false;
  return false;
}
inline bool fields(json_t *object, std::initializer_list<const char *> names, ParseResult &r, const std::string &path) {
  if (!json_is_object(object))
    return error(r, path, "Expected object");
  const char *key;
  json_t *value;
  json_object_foreach(object, key, value) {
    bool allowed = false;
    for (const char *name : names)
      allowed |= std::string(key) == name;
    if (!allowed)
      return error(r, path + "." + key, "Unknown field");
  }
  return true;
}
inline bool id(const std::string &value) {
  return !value.empty() && value.size() <= 64 &&
         std::none_of(value.begin(), value.end(), [](unsigned char c) { return c < 32 || c == 127; });
}
inline bool number(json_t *value, double lo, double hi) {
  return json_is_number(value) && std::isfinite(json_number_value(value)) && json_number_value(value) >= lo &&
         json_number_value(value) <= hi;
}
inline bool integer(json_t *value, int lo, int hi) { return json_is_integer(value) && number(value, lo, hi); }
inline bool note(json_t *value, int &semitone, ParseResult &r, const std::string &path) {
  std::string text = string(value);
  size_t i = 1;
  if (text.empty() || text[0] < 'A' || text[0] > 'G')
    return error(r, path, "Expected scientific note");
  if (i < text.size() && (text[i] == '#' || text[i] == 'b'))
    ++i;
  if (i < text.size() && text[i] == '-')
    ++i;
  if (i == text.size())
    return error(r, path, "Expected scientific-note octave");
  for (; i < text.size(); ++i)
    if (text[i] < '0' || text[i] > '9')
      return error(r, path, "Invalid scientific note");
  if (text.size() > 8)
    return error(r, path, "Note outside supported domain", "pitch_out_of_range");
  float v = noteToPitchV(text, r, path);
  if (!std::isfinite(v) || v < -10 || v > 10)
    return error(r, path, "Note outside -10 to 10 V", "pitch_out_of_range");
  semitone = int(std::lround(v * 12.));
  return true;
}
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
  json_t *chords = json_array();
  for (const auto &c : p.chords) {
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
  for (const auto &name : ids) {
    std::string path = "harmony.progressions." + name;
    value = json_object_get(definitions, name.c_str());
    if (!id(name) || !fields(value, {"lengthBeats", "chords"}, r, path)) {
      error(r, path, "Invalid progression");
      continue;
    }
    HarmonyProgression p;
    p.id = name;
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
      if (!fields(chord, {"id", "beat", "root", "intervals"}, r, cp))
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
      auto inserted = chordDefinitions.emplace(signature, int(chordDefinitions.size()));
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
        auto found = expressions.find(event.harmonic);
        if (found != expressions.end()) {
          event.harmonicExpression = found->second;
          continue;
        }
        json_error_t je;
        json_t *authored = json_loads(event.harmonic.c_str(), 0, &je);
        HarmonicExpression e;
        bool valid = expression(authored, e, comp.meta, r, "patterns." + name + ".notes." + event.id + ".harmonic");
        json_decref(authored);
        if (valid) {
          event.harmonicExpression = int(comp.harmonicExpressions.size());
          expressions[event.harmonic] = event.harmonicExpression;
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
  size_t storage = comp.progressions.capacity() * sizeof(HarmonyProgression) +
                   comp.harmonicExpressions.capacity() * sizeof(HarmonicExpression);
  for (const auto &progression : comp.progressions) {
    storage += progression.id.capacity() + 1 + progression.chords.capacity() * sizeof(HarmonyChord);
    for (const auto &chord : progression.chords)
      storage += chord.id.capacity() + chord.root.capacity() + 2 + chord.intervals.capacity() * sizeof(int);
  }
  for (const auto &curve : comp.automation)
    storage += sizeof(AutomationCurve) + 192 + curve.points.size() * sizeof(AutomationPoint) +
               curve.segments.size() * sizeof(AutomationSegment);
  storage += comp.arrangement.size() * (sizeof(AutomationRoute) + sizeof(double));
  for (const auto &e : comp.harmonicExpressions)
    storage += e.authored.capacity() + 1;
  for (const auto &pattern : comp.patterns)
    for (const auto &event : pattern.second.steps)
      if (event.pitchType == PitchType::HARMONIC)
        storage += event.harmonic.capacity() + 1;
  if (storage > 32u * 1024u * 1024u) {
    error(r, "harmony", "Compiled storage exceeds 32 MiB", "capacity_exceeded");
    return;
  }
  for (size_t s = 0; s < comp.arrangement.size(); ++s) {
    const auto &sc = comp.arrangement[s];
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
    for (const auto &assignment : sc.tracks) {
      auto pat = comp.patterns.find(assignment.second.patternId);
      if (pat == comp.patterns.end())
        continue;
      assigned.insert(pat->first);
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
            auto found = table.find(chord.definition);
            float pitch = 0.;
            if (found == table.end()) {
              if (comp.harmonicPitchEntries >= 1048576 ||
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
            float final = assignment.second.overrides.pitch(pitch + event.transposeSemitones / 12.f);
            if (final < -10.f || final > 10.f)
              error(r, path, "Resolved/transposed pitch outside -10 to 10 V", "pitch_out_of_range");
          }
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
