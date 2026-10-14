#pragma once
#include "SibylEdit.hpp"
#include <set>
namespace sibyl {
inline bool isHarmonyOperation(const std::string &op) {
  return op == "upsert_progression" || op == "delete_progression" || op == "set_default_harmony" ||
         op == "set_scene_harmony" || op == "inherit_scene_harmony";
}
inline bool applyHarmonyOperation(json_t *root, json_t *op, size_t index, EditResult &result) {
  const std::string name = json_string_value(json_object_get(op, "op"));
  const std::string path = "operations[" + std::to_string(index) + "]";
  auto fail = [&](const char *code, const std::string &field, const char *message) {
    result.errorCode = code;
    result.errorPath = path + field;
    result.errorMessage = message;
    return false;
  };
  const bool progression = name == "upsert_progression" || name == "delete_progression";
  const bool scene = name == "set_scene_harmony" || name == "inherit_scene_harmony";
  std::set<std::string> allowed = {"op"};
  if (progression)
    allowed.insert("id");
  if (scene)
    allowed.insert("scene_id");
  if (name == "upsert_progression")
    allowed.insert("progression");
  if (name == "set_default_harmony" || name == "set_scene_harmony")
    allowed.insert("binding");
  const char *key;
  json_t *value;
  json_object_foreach(op, key, value) if (!allowed.count(key)) return fail("invalid_operation", "." + std::string(key),
                                                                           "Unknown field");
  json_t *harmony = json_object_get(root, "harmony");
  if (!harmony) {
    harmony = json_object();
    json_object_set_new(root, "harmony", harmony);
  }
  if (progression) {
    json_t *id = json_object_get(op, "id");
    if (!json_is_string(id) || !json_string_length(id) || json_string_length(id) > 64)
      return fail("invalid_operation", ".id", "Expected progression ID");
    const char *nameId = json_string_value(id);
    json_t *definitions = json_object_get(harmony, "progressions");
    if (!definitions) {
      definitions = json_object();
      json_object_set_new(harmony, "progressions", definitions);
    }
    if (name == "upsert_progression") {
      json_t *p = json_object_get(op, "progression");
      if (!json_is_object(p))
        return fail("invalid_operation", ".progression", "Expected full progression");
      json_object_set(definitions, nameId, p);
      return true;
    }
    if (!json_object_get(definitions, nameId))
      return fail("object_not_found", ".id", "Progression does not exist");
    auto refers = [&](json_t *b) {
      json_t *ref = json_object_get(b, "progression");
      return json_is_string(ref) && std::string(json_string_value(ref)) == nameId;
    };
    if (refers(json_object_get(harmony, "default")))
      return fail("object_in_use", ".id", "Detach default binding before deletion");
    size_t i;
    json_t *s;
    json_array_foreach(json_object_get(root, "arrangement"), i,
                       s) if (refers(json_object_get(s,
                                                     "harmony"))) return fail("object_in_use", ".id",
                                                                              "Detach scene binding before deletion");
    json_object_del(definitions, nameId);
    return true;
  }
  json_t *target = harmony;
  if (scene) {
    json_t *id = json_object_get(op, "scene_id");
    if (!json_is_string(id))
      return fail("invalid_operation", ".scene_id", "Expected scene ID");
    target = nullptr;
    size_t i;
    json_t *s;
    json_array_foreach(json_object_get(root, "arrangement"), i, s) if (json_equal(json_object_get(s, "id"), id))
      target = s;
    if (!target)
      return fail("object_not_found", ".scene_id", "Scene does not exist");
  }
  const char *field = scene ? "harmony" : "default";
  if (name == "inherit_scene_harmony") {
    json_object_del(target, field);
    return true;
  }
  json_t *b = json_object_get(op, "binding");
  if (!json_is_object(b) && !json_is_null(b))
    return fail("invalid_operation", ".binding", "Expected binding object or null");
  if (!scene && json_is_null(b))
    json_object_del(target, field);
  else
    json_object_set(target, field, b);
  return true;
}
} // namespace sibyl
