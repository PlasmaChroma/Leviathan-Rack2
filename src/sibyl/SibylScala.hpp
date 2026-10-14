#pragma once
#include "SibylTuningJSON.hpp"
#include <iomanip>
#include <locale>
#include <sstream>

namespace sibyl {
namespace tuning_json {
inline std::string trim(const std::string &text) {
  size_t a = text.find_first_not_of(" \t\r"), b = text.find_last_not_of(" \t\r");
  return a == std::string::npos ? std::string() : text.substr(a, b - a + 1);
}
// Scala text, never a path. See https://www.huygens-fokker.org/scala/scl_format.html
inline json_t *importScala(const std::string &text, ParseResult &r, const std::string &path) {
  auto failImport = [&](const char *code, const char *message) -> json_t * {
    fail(r, path, message, code);
    return nullptr;
  };
  if (text.size() > 262144)
    return failImport("capacity_exceeded", "Scala source exceeds 256 KiB");
  if (text.find('\0') != std::string::npos)
    return failImport("invalid_scala", "Embedded NUL in Scala source");
  std::istringstream input(text);
  input.imbue(std::locale::classic());
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(input, line)) {
    auto s = trim(line);
    if (!s.empty() && s[0] == '!')
      continue;
    lines.push_back(s);
  }
  if (lines.size() < 2)
    return failImport("invalid_scala", "Missing Scala description or count");
  if (lines[0].size() > 2048)
    return failImport("capacity_exceeded", "Scala description exceeds 2048-byte metadata capacity");
  const auto &countLine = lines[1];
  size_t digits = 0;
  while (digits < countLine.size() && countLine[digits] >= '0' && countLine[digits] <= '9')
    ++digits;
  if (!digits || digits > 9 ||
      (digits < countLine.size() && countLine[digits] != ' ' && countLine[digits] != '\t'))
    return failImport("invalid_scala", "Invalid Scala note count");
  int count = std::stoi(countLine.substr(0, digits));
  if (count < 1 || count > 1024)
    return failImport("unsupported_tuning_shape", "Sibyl supports 1-1024 periodic positions");
  if (lines.size() < size_t(count + 2))
    return failImport("invalid_scala", "Fewer pitch lines than stated count");
  json_t *out = json_pack("{s:s,s:s,s:{s:s}}", "kind", "table", "description", lines[0].c_str(), "source",
                          "format", "scala_scl");
  std::unique_ptr<json_t, void (*)(json_t *)> owner(out, json_decref);
  json_t *positions = json_array();
  json_object_set_new(out, "positions", positions);
  json_array_append_new(positions, json_pack("{s:s}", "ratio", "1/1"));
  double last = 0.;
  for (int i = 0; i < count; ++i) {
    const auto &s = lines[size_t(i) + 2];
    size_t end = s.find_first_of(" \t!");
    std::string token = s.substr(0, end);
    json_t *interval = nullptr;
    double volts = 0.;
    if (token.find('.') != std::string::npos) {
      // A decimal point distinguishes cents from ratios; no locale-dependent conversion.
      std::istringstream numberInput(token);
      numberInput.imbue(std::locale::classic());
      double cents = 0.;
      numberInput >> cents;
      if (numberInput.fail() || !std::isfinite(cents))
        return failImport("invalid_scala", "Malformed cents pitch");
      volts = cents / 1200.;
      interval = json_pack("{s:f}", "cents", cents);
    } else {
      size_t used = 0;
      while (used < token.size() && token[used] >= '0' && token[used] <= '9')
        ++used;
      if (used < token.size() && token[used] == '/') {
        ++used;
        while (used < token.size() && token[used] >= '0' && token[used] <= '9')
          ++used;
      }
      token = token.substr(0, used);
      if (token.find('/') == std::string::npos)
        token += "/1";
      json_t *value = json_string(token.c_str());
      ParseResult check;
      if (!ratio(value, volts, check, path)) {
        json_decref(value);
        return failImport("invalid_scala", "Malformed ratio pitch");
      }
      interval = json_pack("{s:o}", "ratio", value);
    }
    if (volts <= last || !std::isfinite(volts) || (i == count - 1 && (volts < 1. / 1200. || volts > 4.))) {
      json_decref(interval);
      return failImport("unsupported_tuning_shape",
                        "Scala pitches must ascend above unison to a period of 1-4800 cents");
    }
    last = volts;
    if (i == count - 1)
      json_object_set_new(out, "period", interval);
    else
      json_array_append_new(positions, interval);
  }
  return owner.release();
}
inline std::string exportScala(json_t *authored, const PitchTuning &tuning) {
  std::ostringstream out;
  out.imbue(std::locale::classic());
  out << std::fixed << std::setprecision(12);
  std::string description = harmony_json::string(json_object_get(authored, "description"));
  if (description.empty())
    description = harmony_json::string(json_object_get(authored, "name"));
  for (char &c : description)
    if (c == '\r' || c == '\n')
      c = ' ';
  out << "! Sibyl normalized Scala export\n" << description << "\n" << tuning.divisions << "\n";
  auto pitch = [&](json_t *interval, double volts) {
    json_t *ratio = json_object_get(interval, "ratio");
    if (ratio)
      out << harmony_json::string(ratio);
    else
      out << volts * 1200.;
    out << '\n';
  };
  for (int i = 1; i < tuning.divisions; ++i)
    pitch(json_array_get(json_object_get(authored, "positions"), size_t(i)), tuning.lattice(i));
  pitch(json_object_get(authored, "period"), tuning.periodV);
  return out.str();
}
} // namespace tuning_json
} // namespace sibyl
