#pragma once

#include "ChimeraProfile.hpp"
#include <iomanip>
#include <locale>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace chimera {
namespace optionsText {

struct Values {
    int ckop = 0, vsop = 0, inop = 0, pmin = 0, omod = 0;
    int gnsm = 0, rsop = 0, pmod = 0, cvop = 0;
    float mcr[3] = {2.f, 1.5f, 4.f/3.f};
};

struct Result {
    bool valid = false;
    Values values;
    std::map<std::string, std::string> extras;
    std::vector<std::string> warnings;
    std::string error;
    std::size_t line = 0;
};

inline bool safeKey(const std::string& key) {
    if (key.empty() || key.size() > 64 ||
        !((key[0] >= 'a' && key[0] <= 'z') ||
          (key[0] >= 'A' && key[0] <= 'Z'))) return false;
    for (char c : key)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_')) return false;
    return true;
}
inline bool safeValue(const std::string& value) {
    if (value.empty() || value.size() > 128) return false;
    for (char c : value)
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '.' ||
              c == '+' || c == '-')) return false;
    return true;
}
inline bool recognizedKey(const std::string& key) {
    return key == "ckop" || key == "vsop" || key == "inop" || key == "pmin" ||
           key == "omod" || key == "gnsm" || key == "rsop" || key == "pmod" ||
           key == "cvop" || key == "mcr1" || key == "mcr2" || key == "mcr3";
}
template <typename T>
inline bool number(const std::string& token, T& value) {
    std::istringstream stream(token);
    stream.imbue(std::locale::classic());
    char extra;
    return static_cast<bool>(stream >> value) && !(stream >> extra);
}

inline Result parse(const std::string& text, const Values& base = Values()) {
    Result result;
    result.values = base;
    if (text.size() > 65536) { result.error = "Options text exceeds 64 KiB"; return result; }
    std::set<std::string> seen;
    std::istringstream lines(text);
    std::string line;
    while (std::getline(lines, line)) {
        ++result.line;
        if (result.line > 1024) { result.error = "Too many options lines"; return result; }
        const std::size_t comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        std::istringstream fields(line);
        std::string key, value, extra;
        if (!(fields >> key)) continue;
        if (!(fields >> value) || (fields >> extra)) {
            result.error = "Expected one key and one value";
            return result;
        }
        if (!safeKey(key) || !safeValue(value)) {
            result.error = "Unsafe or oversized option token";
            return result;
        }
        if (!seen.insert(key).second) { result.error = "Duplicate option: " + key; return result; }
        int* enumTarget = nullptr;
        int maxValue = 1;
        if (key == "ckop") { enumTarget = &result.values.ckop; maxValue = 2; }
        else if (key == "vsop") { enumTarget = &result.values.vsop; maxValue = 2; }
        else if (key == "inop") enumTarget = &result.values.inop;
        else if (key == "pmin") enumTarget = &result.values.pmin;
        else if (key == "omod") enumTarget = &result.values.omod;
        else if (key == "gnsm") enumTarget = &result.values.gnsm;
        else if (key == "rsop") enumTarget = &result.values.rsop;
        else if (key == "pmod") { enumTarget = &result.values.pmod; maxValue = 2; }
        else if (key == "cvop") enumTarget = &result.values.cvop;
        if (enumTarget) {
            int parsed = 0;
            if (!number(value, parsed) || parsed < 0 || parsed > maxValue) {
                result.error = "Invalid value for " + key;
                return result;
            }
            *enumTarget = parsed;
        }
        else if (key == "mcr1" || key == "mcr2" || key == "mcr3") {
            double parsed = 0;
            if (!number(value, parsed) || !profile1::finite(parsed) ||
                std::fabs(parsed) < 0.0625 || std::fabs(parsed) > 16.0) {
                result.error = "Invalid value for " + key;
                return result;
            }
            result.values.mcr[key[3] - '1'] = static_cast<float>(parsed);
        }
        else {
            if (result.extras.size() >= 64) {
                result.error = "Too many unknown options";
                return result;
            }
            result.extras[key] = value;
            result.warnings.push_back("Unknown option retained: " + key);
        }
    }
    result.valid = true;
    result.line = 0;
    return result;
}

inline std::string exportText(const Values& v,
                              const std::map<std::string, std::string>& extras = {}) {
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out << std::setprecision(9);
    out << "ckop " << v.ckop << '\n' << "vsop " << v.vsop << '\n'
        << "inop " << v.inop << '\n' << "pmin " << v.pmin << '\n'
        << "omod " << v.omod << '\n' << "gnsm " << v.gnsm << '\n'
        << "rsop " << v.rsop << '\n' << "pmod " << v.pmod << '\n'
        << "cvop " << v.cvop << '\n';
    for (int i = 0; i < 3; ++i) out << "mcr" << (i + 1) << ' ' << v.mcr[i] << '\n';
    for (const auto& extra : extras) out << extra.first << ' ' << extra.second << '\n';
    return out.str();
}

} // namespace optionsText
} // namespace chimera
