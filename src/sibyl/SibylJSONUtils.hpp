#pragma once
#include "SibylJSON.hpp"
#include <climits>
#include <algorithm>
#include <cmath>
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
}}
