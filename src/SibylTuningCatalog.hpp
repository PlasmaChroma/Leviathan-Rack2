#pragma once
#include "SibylScala.hpp"
#include <set>

namespace sibyl {
inline json_t *tuningCatalog() {
  json_t *entries = json_object();
  for (int n : {5, 7, 12, 17, 19, 22, 24, 31, 38, 41, 48, 53, 72}) {
    auto id = std::to_string(n) + "edo";
    auto name = std::to_string(n) + " equal divisions of the octave";
    json_object_set_new(entries, id.c_str(),
                        json_pack("{s:s,s:i,s:{s:s},s:s}", "kind", "equal", "divisions", n, "period", "ratio",
                                  "2/1", "name", name.c_str()));
  }
  json_object_set_new(entries, "13edt",
                      json_pack("{s:s,s:i,s:{s:s},s:s}", "kind", "equal", "divisions", 13, "period", "ratio",
                                "3/1", "name", "13 equal divisions of the tritave"));
  json_t *positions = json_array();
  for (const char *ratio : {"1/1", "9/8", "5/4", "4/3", "3/2", "5/3", "15/8"})
    json_array_append_new(positions, json_pack("{s:s}", "ratio", ratio));
  json_object_set_new(entries, "just7",
                      json_pack("{s:s,s:{s:s},s:o,s:s}", "kind", "table", "period", "ratio", "2/1",
                                "positions", positions, "name", "Seven-position just-ratio table"));
  return entries;
}
inline std::string serializeTuningAssistance(const Composition &comp, json_t *request) {
  using namespace harmony_json;
  auto fail = [](const char *code, const char *message) {
    json_t *out = json_pack("{s:b,s:{s:s,s:s}}", "ok", 0, "error", "code", code, "message", message);
    auto text = dump(out);
    json_decref(out);
    return text;
  };
  const auto view = string(json_object_get(request, "view"));
  if (json_object_get(request, "id") && !json_is_string(json_object_get(request, "id")))
    return fail("invalid_request", "id must be a string");
  ParseResult validation;
  bool valid = view == "map_intervals"
                   ? fields(request, {"view", "context_id", "intervals", "tie_break"}, validation, "request")
                   : fields(request, {"view", "id"}, validation, "request");
  if (!valid)
    return fail("invalid_request", "Unknown query field");
  json_t *out = json_pack("{s:b,s:s,s:i}", "ok", 1, "view", view.c_str(), "revision", comp.revision);
  std::unique_ptr<json_t, void (*)(json_t *)> owner(out, json_decref);
  if (view == "tuning_catalog") {
    json_t *catalog = tuningCatalog();
    std::unique_ptr<json_t, void (*)(json_t *)> c(catalog, json_decref);
    const auto id = string(json_object_get(request, "id"));
    if (id.empty())
      json_object_set(out, "definitions", catalog);
    else {
      json_t *tuning = json_object_get(catalog, id.c_str());
      if (!tuning)
        return fail("object_not_found", "Unknown catalog ID");
      json_object_set(out, "tuning", tuning);
    }
  } else if (view == "export_tuning_scl") {
    auto id = string(json_object_get(request, "id"));
    auto t = comp.pitchSystems.tunings.find(id);
    if (t == comp.pitchSystems.tunings.end())
      return fail("object_not_found", "Unknown tuning ID");
    json_t *definitions = json_loads(comp.pitchSystems.authored.c_str(), 0, nullptr);
    auto text = tuning_json::exportScala(json_object_get(json_object_get(definitions, "tunings"), id.c_str()),
                                         t->second);
    json_decref(definitions);
    json_object_set_new(out, "text", json_string(text.c_str()));
  } else if (view == "map_intervals") {
    const auto contextId = string(json_object_get(request, "context_id"));
    auto c = comp.pitchSystems.contexts.find(contextId);
    if (c == comp.pitchSystems.contexts.end())
      return fail("unresolved_pitch_context", "Unknown context");
    const auto &tuning = comp.pitchSystems.tunings.at(c->second.tuning);
    auto tie = string(json_object_get(request, "tie_break"));
    if (json_object_get(request, "tie_break") && tie != "lower" && tie != "higher")
      return fail("invalid_request", "tie_break must be lower or higher");
    json_t *intervals = json_object_get(request, "intervals");
    if (!json_is_array(intervals) || json_array_size(intervals) < 1 || json_array_size(intervals) > 1024)
      return fail("invalid_request", "Expected 1-1024 intervals");
    json_t *mapped = json_array();
    json_object_set_new(out, "intervals", mapped);
    json_t *collisions = json_array();
    json_object_set_new(out, "collisions", collisions);
    std::map<int64_t, size_t> seen;
    size_t i;
    json_t *interval;
    json_array_foreach(intervals, i, interval) {
      double target = 0.;
      json_t *copy = json_deep_copy(interval);
      bool ok = tuning_json::interval(copy, target, validation, "intervals");
      json_decref(copy);
      if (!ok)
        return fail("invalid_request", "Malformed target interval");
      int64_t period = int64_t(std::floor(target / tuning.periodV)), selected = 0;
      double best = std::numeric_limits<double>::infinity(), realized = 0.;
      for (int64_t q = period - 1; q <= period + 1; ++q)
        for (int k = 0; k < tuning.divisions; ++k) {
          int64_t step = q * tuning.divisions + k;
          double v = tuning.lattice(step), distance = std::abs(v - target);
          if (distance < best - 1e-12 ||
              (std::abs(distance - best) <= 1e-12 && (tie == "higher" ? step > selected : step < selected))) {
            best = distance;
            selected = step;
            realized = v;
          }
        }
      json_array_append_new(mapped,
                            json_pack("{s:O,s:I,s:f,s:f,s:f}", "target", interval, "step",
                                      json_int_t(selected), "targetCents", target * 1200., "realizedCents",
                                      realized * 1200., "errorCents", (realized - target) * 1200.));
      auto found = seen.emplace(selected, i);
      if (!found.second)
        json_array_append_new(collisions,
                              json_pack("{s:I,s:I,s:I}", "step", json_int_t(selected), "firstIndex",
                                        json_int_t(found.first->second), "index", json_int_t(i)));
    }
    if (json_array_size(collisions)) {
      json_object_set_new(out, "ok", json_false());
      json_object_set_new(out, "error",
                          json_pack("{s:s,s:s}", "code", "scale_mapping_collision", "message",
                                    "Distinct requested intervals map to the same step"));
    }
  } else
    return fail("invalid_request", "Unknown tuning assistance view");
  return dump(out);
}
} // namespace sibyl
