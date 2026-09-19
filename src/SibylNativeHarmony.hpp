#pragma once
#include "SibylTuningJSON.hpp"
#include <set>

namespace sibyl {
namespace harmony_json {
// Static endpoints retain double precision; only legacy degree notation uses the old scale.
inline bool staticPitch(json_t *value, const std::string &context, const Composition &comp,
                        NativePitch &pitch, ParseResult &r, const std::string &path,
                        bool degreeAllowed = false) {
  if (json_is_string(value)) {
    int n = 0;
    if (!note(value, n, r, path))
      return false;
    pitch.baseV = n / 12.;
  } else {
    if (!fields(value, {"tuned", "note", "pitchV", "degree", "octave"}, r, path))
      return false;
    json_t *t = json_object_get(value, "tuned"), *n = json_object_get(value, "note"),
           *v = json_object_get(value, "pitchV"), *d = json_object_get(value, "degree"),
           *o = json_object_get(value, "octave");
    if (bool(t) + bool(n) + bool(v) + bool(d) != 1 || (o && !d) || (d && !degreeAllowed))
      return error(r, path, "Expected one static pitch");
    if (t) {
      if (!tuning_json::resolve(t, context, comp.pitchSystems, pitch.baseV, r, path + ".tuned", &pitch))
        return false;
      return pitchInDomain(pitch.baseV) || error(r, path, "Pitch outside domain", "pitch_out_of_range");
    }
    if (n) {
      int semitone = 0;
      if (!note(n, semitone, r, path + ".note"))
        return false;
      pitch.baseV = semitone / 12.;
    }
    if (v) {
      if (!number(v, -10, 10))
        return error(r, path, "Pitch outside domain", "pitch_out_of_range");
      pitch.baseV = json_number_value(v);
    }
    if (d) {
      if (!integer(d, INT_MIN, INT_MAX) || (o && !integer(o, INT_MIN, INT_MAX)))
        return error(r, path, "Invalid degree");
      pitch.baseV = degreeToPitchV(int(json_integer_value(d)), int(json_integer_value(o)), comp.meta.scale,
                                   comp.meta.root, comp.meta.rootOctave);
    }
  }
  if (!context.empty()) {
    auto c = comp.pitchSystems.contexts.find(context);
    if (c == comp.pitchSystems.contexts.end())
      return error(r, path, "Undefined pitch context", "unresolved_pitch_context");
    pitch.context = context;
    pitch.periodV = comp.pitchSystems.tunings.at(c->second.tuning).periodV;
  }
  return pitchInDomain(pitch.baseV) || error(r, path, "Pitch outside domain", "pitch_out_of_range");
}
inline bool nativeChord(json_t *value, const std::string &context, const Composition &comp,
                        HarmonyChord &chord, ParseResult &r, const std::string &path) {
  auto c = comp.pitchSystems.contexts.find(context);
  if (c == comp.pitchSystems.contexts.end())
    return error(r, path, "Native progression requires a valid pitchContext", "unresolved_pitch_context");
  chord.context = context;
  chord.periodV = comp.pitchSystems.tunings.at(c->second.tuning).periodV;
  NativePitch root;
  json_t *rootJson = json_object_get(value, "rootPitch");
  if (!json_is_object(rootJson) || !staticPitch(rootJson, context, comp, root, r, path + ".rootPitch"))
    return false;
  if (root.context != context)
    return error(r, path, "Root context must match progression");
  json_t *tones = json_object_get(value, "tones");
  if (!json_is_array(tones) || json_array_size(tones) < 1 || json_array_size(tones) > 8)
    return error(r, path, "Expected 1-8 native tones");
  std::set<std::string> ids, roles;
  double last = -1.;
  size_t i;
  json_t *t;
  json_array_foreach(tones, i, t) {
    if (!fields(t, {"id", "interval", "roles"}, r, path + ".tones"))
      return false;
    NativeChordTone tone;
    tone.id = string(json_object_get(t, "id"));
    tone.pitch = root;
    if (!id(tone.id) || !ids.insert(tone.id).second)
      return error(r, path, "Tone IDs must be unique");
    json_t *interval = json_object_get(t, "interval");
    if (!fields(interval, {"steps", "cents", "ratio"}, r, path + ".interval") ||
        json_object_size(interval) != 1)
      return error(r, path, "Expected one tone interval");
    double delta = 0.;
    json_t *steps = json_object_get(interval, "steps"), *cents = json_object_get(interval, "cents");
    if (steps) {
      if (!root.lattice || !integer(steps, INT_MIN, INT_MAX))
        return error(r, path, "Step intervals require a lattice root", "unsupported_pitch_transform");
      tone.pitch.index += json_integer_value(steps);
      tone.pitch.baseV =
          c->second.anchorV + comp.pitchSystems.tunings.at(c->second.tuning).lattice(tone.pitch.index);
      delta = tone.pitch.baseV - root.baseV;
    } else {
      tone.pitch.lattice = false;
      if (cents) {
        if (!number(cents, -24000, 24000))
          return error(r, path, "Invalid cents interval");
        delta = json_number_value(cents) / 1200.;
      } else if (!tuning_json::ratio(json_object_get(interval, "ratio"), delta, r, path + ".ratio"))
        return false;
      tone.pitch.baseV += delta;
    }
    if ((i == 0 ? std::abs(delta) > 1e-7 / 1200. : delta - last <= 1e-7 / 1200.) || delta < 0 ||
        delta >= chord.periodV)
      return error(r, path, "Tone intervals must start at zero and ascend uniquely within one period");
    last = delta;
    json_t *roleArray = json_object_get(t, "roles");
    if (roleArray && (!json_is_array(roleArray) || json_array_size(roleArray) > 64))
      return error(r, path, "Expected bounded role array");
    size_t k;
    json_t *role;
    json_array_foreach(roleArray, k, role) {
      auto name = string(role);
      if (!id(name) || !roles.insert(name).second || (name == "root" && i != 0))
        return error(r, path, "Roles must be unique; root belongs to first tone");
      tone.roles.push_back(name);
    }
    chord.tones.push_back(std::move(tone));
  }
  if (!roles.count("root"))
    return error(r, path, "First tone requires root role");
  chord.authored = dump(value);
  return true;
}
inline bool nativeExpression(json_t *value, HarmonicExpression &e, const Composition &comp,
                             const std::string &context, ParseResult &r, const std::string &path) {
  e.authored = dump(value);
  e.extended = true;
  const auto kind = string(json_object_get(value, "kind"));
  if (kind == "tone") {
    if (!fields(value, {"kind", "index", "role", "toneId", "octave", "periods"}, r, path))
      return false;
    json_t *index = json_object_get(value, "index"), *role = json_object_get(value, "role"),
           *tone = json_object_get(value, "toneId"), *oct = json_object_get(value, "octave"),
           *period = json_object_get(value, "periods");
    if (bool(index) + bool(role) + bool(tone) != 1 || (oct && period))
      return error(r, path, "Select exactly one index, role or toneId; octave and periods are exclusive");
    if ((index && !integer(index, 0, 7)) || (oct && !integer(oct, -10, 10)) ||
        (period && !integer(period, INT_MIN, INT_MAX)))
      return error(r, path, "Invalid tone coordinate");
    e.index = int(json_integer_value(index));
    e.octave = int(json_integer_value(oct));
    e.hasPeriods = period;
    e.periods = int32_t(json_integer_value(period));
    e.roleName = string(role);
    e.toneId = string(tone);
    if ((role && !id(e.roleName)) || (tone && !id(e.toneId)))
      return error(r, path, "Invalid tone or role ID");
  } else if (kind == "nearest") {
    e.nearest = true;
    if (!fields(value, {"kind", "reference", "range", "tieBreak"}, r, path))
      return false;
    json_t *range = json_object_get(value, "range");
    if (!fields(range, {"min", "max"}, r, path + ".range"))
      return false;
    NativePitch ref, lo, hi;
    if (!staticPitch(json_object_get(value, "reference"), context, comp, ref, r, path + ".reference", true) ||
        !staticPitch(json_object_get(range, "min"), context, comp, lo, r, path + ".range.min", true) ||
        !staticPitch(json_object_get(range, "max"), context, comp, hi, r, path + ".range.max", true))
      return false;
    e.referenceV = ref.baseV;
    e.minimumV = lo.baseV;
    e.maximumV = hi.baseV;
    e.reference = e.referenceV * 12.;
    auto tie = string(json_object_get(value, "tieBreak"));
    if ((json_object_get(value, "tieBreak") && tie != "lower" && tie != "higher") ||
        e.minimumV > e.maximumV)
      return error(r, path, "Invalid nearest range or tieBreak");
    e.higher = tie == "higher";
  } else
    return error(r, path, "Expected tone or nearest");
  return true;
}
inline bool resolveNative(const HarmonicExpression &e, const HarmonyChord &chord, const PitchSystems &systems,
                          NativePitch &selected, std::string *selectedId = nullptr) {
  if (chord.tones.empty())
    return false;
  bool found = false;
  double best = std::numeric_limits<double>::infinity();
  std::string bestId;
  for (size_t i = 0; i < chord.tones.size(); ++i) {
    const auto &tone = chord.tones[i];
    if (!e.nearest) {
      std::string role = e.roleName.empty()
                             ? (e.role == 1 ? "root" : e.role == 2 ? "third" : e.role == 3 ? "fifth" : "")
                             : e.roleName;
      bool match = !e.toneId.empty()
                       ? e.toneId == tone.id
                       : !role.empty()
                             ? std::find(tone.roles.begin(), tone.roles.end(), role) != tone.roles.end()
                             : int(i) == e.index;
      if (!match)
        continue;
      selected = tone.pitch;
      if (e.hasPeriods) {
        selected.baseV += double(e.periods) * chord.periodV;
        if (selected.lattice)
          selected.index +=
              int64_t(e.periods) * systems.tunings.at(systems.contexts.at(chord.context).tuning).divisions;
      } else if (e.octave) {
        selected.baseV += e.octave;
        if (selected.lattice && chord.periodV == 1.)
          selected.index +=
              int64_t(e.octave) * systems.tunings.at(systems.contexts.at(chord.context).tuning).divisions;
        else
          selected.lattice = false;
      }
      if (selectedId)
        *selectedId = tone.id;
      return true;
    }
    const double lo = e.extended ? e.minimumV : e.minimum / 12.,
                 hi = e.extended ? e.maximumV : e.maximum / 12.,
                 ref = e.extended ? e.referenceV : e.reference / 12.;
    const double first = std::ceil((lo - tone.pitch.baseV) / chord.periodV),
                 last = std::floor((hi - tone.pitch.baseV) / chord.periodV);
    if (first > last)
      continue;
    double ideal = std::floor((ref - tone.pitch.baseV) / chord.periodV);
    for (int k = 0; k < 2; ++k) {
      double j = std::max(first, std::min(last, ideal + k));
      double v = tone.pitch.baseV + j * chord.periodV, distance = std::abs(v - ref);
      if (v < lo || v > hi)
        continue;
      bool tied = std::abs(distance - best) <= 1e-12;
      if (!found || distance < best - 1e-12 ||
          (tied && ((e.higher ? v > selected.baseV : v < selected.baseV) ||
                    (v == selected.baseV && tone.id < bestId)))) {
        found = true;
        best = distance;
        bestId = tone.id;
        selected = tone.pitch;
        selected.baseV = v;
        if (selected.lattice)
          selected.index +=
              int64_t(j) * systems.tunings.at(systems.contexts.at(chord.context).tuning).divisions;
      }
    }
  }
  if (selectedId)
    *selectedId = bestId;
  return found;
}
} // namespace harmony_json
} // namespace sibyl
