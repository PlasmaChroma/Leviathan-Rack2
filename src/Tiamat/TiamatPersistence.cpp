#include "TiamatPersistence.hpp"
#include <cmath>
#include <limits>

namespace tiamat {
bool parseSeed(const std::string& text, std::uint64_t& result) noexcept {
    if (text.empty()) return false;
    unsigned base = 10, start = 0;
    if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) { base = 16; start = 2; }
    std::uint64_t value = 0;
    for (unsigned i = start; i < text.size(); ++i) {
        const char c = text[i]; unsigned digit = c >= '0' && c <= '9' ? unsigned(c - '0')
            : c >= 'a' && c <= 'f' ? unsigned(c - 'a' + 10) : c >= 'A' && c <= 'F' ? unsigned(c - 'A' + 10) : 99;
        if (digit >= base || value > (std::numeric_limits<std::uint64_t>::max() - digit) / base) return false;
        value = value * base + digit;
    }
    result = value; return true;
}
namespace {
float number(json_t* root, const char* key, float fallback) {
    auto* value = json_object_get(root, key);
    return json_is_number(value) && std::isfinite(json_number_value(value))
        ? float(std::max(0.0, std::min(1.0, json_number_value(value)))) : fallback;
}
bool boolean(json_t* root, const char* key, bool fallback) {
    auto* value = json_object_get(root, key); return json_is_boolean(value) ? json_is_true(value) : fallback;
}
int enumeration(json_t* root, const char* key, int first, int last, int fallback) {
    auto* value = json_object_get(root, key); if (!json_is_integer(value)) return fallback;
    const auto n = json_integer_value(value); return n >= first && n <= last ? int(n) : fallback;
}
}
json_t* presetToJson(const Preset& preset) {
    auto* root = json_object(); const auto& c = preset.controls; const auto& s = preset.settings;
    auto integer = [&](const char* key, int v) { json_object_set_new(root, key, json_integer(v)); };
    auto flag = [&](const char* key, bool v) { json_object_set_new(root, key, json_boolean(v)); };
    auto value = [&](const char* key, float v) { json_object_set_new(root, key, json_real(normalized(v))); };
    integer("schema", schemaVersion); integer("algorithm", algorithmVersion);
    integer("mode", int(c.mode)); integer("clockSource", int(c.clockSource)); integer("effect", int(c.effect));
    flag("macroBend", c.macroBend); flag("macroBreak", c.macroBreak); flag("microReverse", c.microReverse); flag("microSilence", c.microSilence);
    json_object_set_new(root, "seed", json_string(std::to_string(c.seed).c_str()));
    value("window", s.window); value("separation", s.separation); value("bendDepth", s.bendDepth); value("breakDepth", s.breakDepth); value("corruptDepth", s.corruptDepth);
    flag("unique", s.unique); integer("gates", int(s.gates)); integer("freezeButton", int(s.freezeButton));
    integer("corruptGate", int(s.corruptGate)); integer("effectSet", int(s.effectSet));
    return root;
}
Preset presetFromJson(json_t* root) {
    Preset p;
    if (!json_is_object(root)) return p;
    for (const char* key : {"schema", "algorithm"}) {
        auto* v = json_object_get(root, key);
        if (v && (!json_is_integer(v) || json_integer_value(v) != 1)) return p;
    }
    auto& c = p.controls; auto& s = p.settings;
    c.mode = Mode(enumeration(root,"mode",0,1,0)); c.clockSource = ClockSource(enumeration(root,"clockSource",0,1,0));
    c.effect = Effect(enumeration(root,"effect",1,5,1));
    c.macroBend = boolean(root,"macroBend",false); c.macroBreak = boolean(root,"macroBreak",false);
    c.microReverse = boolean(root,"microReverse",false); c.microSilence = boolean(root,"microSilence",false);
    auto* seed = json_object_get(root,"seed");
    if (json_is_string(seed)) parseSeed(json_string_value(seed),c.seed);
    else if (json_is_integer(seed) && json_integer_value(seed) >= 0) c.seed = std::uint64_t(json_integer_value(seed));
    s.window = number(root,"window",s.window); s.separation = number(root,"separation",s.separation);
    s.bendDepth = number(root,"bendDepth",1); s.breakDepth = number(root,"breakDepth",1); s.corruptDepth = number(root,"corruptDepth",1);
    s.unique = boolean(root,"unique",true); s.gates = GateBehavior(enumeration(root,"gates",0,1,0));
    s.freezeButton = FreezeButton(enumeration(root,"freezeButton",0,1,0)); s.corruptGate = CorruptGate(enumeration(root,"corruptGate",0,1,0));
    s.effectSet = EffectSet(enumeration(root,"effectSet",0,1,0));
    if (s.effectSet == EffectSet::OriginalThree && int(c.effect) > 3) c.effect = Effect::Decimate;
    return p;
}
}
