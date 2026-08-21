#include "SibylJSON.hpp"
#include <jansson.h>
#include <cmath>
#include <set>
#include <regex>

namespace sibyl {

static void addError(ParseResult& res, const std::string& path, const std::string& msg) {
    res.errors.push_back({path, msg});
    res.valid = false;
}

static void addWarning(ParseResult& res, const std::string& path, const std::string& msg) {
    res.warnings.push_back({path, msg});
}

// Helpers for reading json
static std::string getString(json_t* obj, const char* key, const std::string& def = "") {
    json_t* val = json_object_get(obj, key);
    if (json_is_string(val)) return json_string_value(val);
    return def;
}

static float getNumber(json_t* obj, const char* key, float def = 0.0f) {
    json_t* val = json_object_get(obj, key);
    if (json_is_number(val)) return json_number_value(val);
    return def;
}

static int getInteger(json_t* obj, const char* key, int def = 0) {
    json_t* val = json_object_get(obj, key);
    if (json_is_integer(val)) return json_integer_value(val);
    return def;
}

static bool getBoolean(json_t* obj, const char* key, bool def = false) {
    json_t* val = json_object_get(obj, key);
    if (json_is_boolean(val)) return json_boolean_value(val);
    return def;
}

static ScaleType parseScale(const std::string& s) {
    if (s == "major") return ScaleType::MAJOR;
    if (s == "natural_minor") return ScaleType::NATURAL_MINOR;
    if (s == "harmonic_minor") return ScaleType::HARMONIC_MINOR;
    if (s == "melodic_minor") return ScaleType::MELODIC_MINOR;
    if (s == "dorian") return ScaleType::DORIAN;
    if (s == "phrygian") return ScaleType::PHRYGIAN;
    if (s == "lydian") return ScaleType::LYDIAN;
    if (s == "mixolydian") return ScaleType::MIXOLYDIAN;
    if (s == "locrian") return ScaleType::LOCRIAN;
    if (s == "major_pentatonic") return ScaleType::MAJOR_PENTATONIC;
    if (s == "minor_pentatonic") return ScaleType::MINOR_PENTATONIC;
    return ScaleType::CHROMATIC;
}

static ApplyAt parseApplyAt(const std::string& s) {
    if (s == "immediate") return ApplyAt::IMMEDIATE;
    if (s == "nextStep") return ApplyAt::NEXT_STEP;
    if (s == "nextScene") return ApplyAt::NEXT_SCENE;
    return ApplyAt::NEXT_BEAT;
}

static PhaseMode parsePhaseMode(const std::string& s) {
    if (s == "continue") return PhaseMode::CONTINUE;
    if (s == "alignGlobal") return PhaseMode::ALIGN_GLOBAL;
    return PhaseMode::RESTART;
}

static OnExternalStop parseOnExternalStop(const std::string& s) {
    if (s == "freeRun") return OnExternalStop::FREE_RUN;
    if (s == "internal") return OnExternalStop::INTERNAL;
    return OnExternalStop::HOLD;
}

// Convert resolution string to beats (quarter notes). e.g. "1/16" -> 0.25
static double parseResolution(const std::string& s, ParseResult& res, const std::string& path) {
    double base = 1.0;
    if (s.substr(0, 2) == "1/") {
        std::string denomStr = s.substr(2);
        bool dotted = false;
        bool triplet = false;
        if (!denomStr.empty() && denomStr.back() == 'd') {
            dotted = true;
            denomStr.pop_back();
        } else if (!denomStr.empty() && denomStr.back() == 't') {
            triplet = true;
            denomStr.pop_back();
        }
        
        try {
            int denom = std::stoi(denomStr);
            if (denom > 0) {
                // 1/4 is 1 beat. So base = 4.0 / denom.
                base = 4.0 / denom;
                if (dotted) base *= 1.5;
                if (triplet) base *= (2.0 / 3.0);
                return base;
            }
        } catch (...) {}
    }
    addError(res, path, "Invalid resolution format: " + s);
    return 0.25; // default 1/16
}

static float noteToPitchV(const std::string& noteStr, ParseResult& res, const std::string& path) {
    // Basic scientific pitch parser. C4 = 0V. 1V/Octave.
    if (noteStr.empty()) return 0.0f;
    char name = noteStr[0];
    int offset = 0;
    size_t numStart = 1;
    if (noteStr.length() > 1) {
        if (noteStr[1] == 'b') { offset = -1; numStart = 2; }
        else if (noteStr[1] == '#') { offset = 1; numStart = 2; }
    }
    
    int octave = 4;
    if (numStart < noteStr.length()) {
        try { octave = std::stoi(noteStr.substr(numStart)); }
        catch (...) { addError(res, path, "Invalid octave in note: " + noteStr); }
    }
    
    int pc = 0;
    switch(name) {
        case 'C': pc = 0; break;
        case 'D': pc = 2; break;
        case 'E': pc = 4; break;
        case 'F': pc = 5; break;
        case 'G': pc = 7; break;
        case 'A': pc = 9; break;
        case 'B': pc = 11; break;
        default: addError(res, path, "Invalid note name: " + noteStr); break;
    }
    
    int totalSemitones = pc + offset + (octave - 4) * 12;
    return totalSemitones / 12.0f;
}

ParseResult parseCompositionJson(const std::string& jsonString, int revision) {
    ParseResult res;
    res.valid = true;
    res.composition = std::make_shared<Composition>();
    Composition& comp = *const_cast<Composition*>(res.composition.get());
    comp.revision = revision;

    json_error_t error;
    json_t* root = json_loads(jsonString.c_str(), 0, &error);
    if (!root) {
        addError(res, "$", "Failed to parse JSON: " + std::string(error.text));
        return res;
    }

    // Parse Meta
    json_t* metaJ = json_object_get(root, "meta");
    if (metaJ) {
        comp.meta.title = getString(metaJ, "title", "Untitled");
        comp.meta.prompt = getString(metaJ, "prompt");
        comp.meta.bpm = getNumber(metaJ, "bpm", 120.0f);
        comp.meta.root = getString(metaJ, "root", "C");
        comp.meta.rootOctave = getInteger(metaJ, "rootOctave", 4);
        comp.meta.scale = parseScale(getString(metaJ, "scale", "chromatic"));
        comp.meta.swing = getNumber(metaJ, "swing", 0.0f);
        json_t* seedJ = json_object_get(metaJ, "seed");
        if (seedJ && json_is_integer(seedJ)) {
            comp.meta.seed = json_integer_value(seedJ);
        }
    }

    // Parse Clock
    json_t* clockJ = json_object_get(root, "clock");
    if (clockJ) {
        comp.clock.externalPpqn = getInteger(clockJ, "externalPpqn", 24);
        comp.clock.outputPpqn = getInteger(clockJ, "outputPpqn", 24);
        comp.clock.externalTimeoutMs = getNumber(clockJ, "externalTimeoutMs", 2000.0f);
        comp.clock.onExternalStop = parseOnExternalStop(getString(clockJ, "onExternalStop", "hold"));
    }

    // Parse Transport
    json_t* transportJ = json_object_get(root, "transport");
    if (transportJ) {
        comp.transport.running = getBoolean(transportJ, "running", true);
        comp.transport.loop = getBoolean(transportJ, "loop", true);
        comp.transport.defaultApplyAt = parseApplyAt(getString(transportJ, "defaultApplyAt", "nextBeat"));
    }

    // Parse Tracks
    json_t* tracksJ = json_object_get(root, "tracks");
    if (tracksJ && json_is_array(tracksJ)) {
        size_t index; json_t* value;
        std::set<int> seenChannels;
        std::set<std::string> seenIds;
        json_array_foreach(tracksJ, index, value) {
            TrackDef t;
            t.id = getString(value, "id");
            t.channel = getInteger(value, "channel", 0);
            t.defaultGate = getNumber(value, "defaultGate", 0.5f);
            t.defaultVelocity = getNumber(value, "defaultVelocity", 0.5f);
            t.modRange = getString(value, "modRange") == "bipolar" ? ModRange::BIPOLAR : ModRange::UNIPOLAR;
            
            std::string path = "tracks[" + std::to_string(index) + "]";
            if (t.id.empty()) addError(res, path, "Track id is missing");
            if (seenIds.count(t.id)) addError(res, path, "Duplicate track id: " + t.id);
            if (seenChannels.count(t.channel)) addError(res, path, "Duplicate channel: " + std::to_string(t.channel));
            if (t.channel < 0 || t.channel >= 16) addError(res, path, "Channel out of bounds (0-15): " + std::to_string(t.channel));
            
            seenIds.insert(t.id);
            seenChannels.insert(t.channel);
            comp.tracks.push_back(t);
        }
    }

    // Parse Patterns
    json_t* patternsJ = json_object_get(root, "patterns");
    if (patternsJ && json_is_object(patternsJ)) {
        const char* key; json_t* val;
        json_object_foreach(patternsJ, key, val) {
            Pattern p;
            p.id = key;
            p.length = getInteger(val, "length", 16);
            p.resolutionStr = getString(val, "resolution", "1/16");
            p.resolutionBeats = parseResolution(p.resolutionStr, res, "patterns." + std::string(key) + ".resolution");
            
            json_t* stepsJ = json_object_get(val, "steps");
            if (stepsJ && json_is_array(stepsJ)) {
                size_t idx; json_t* stepJ;
                std::set<int> seenSteps;
                json_array_foreach(stepsJ, idx, stepJ) {
                    StepEvent e;
                    e.step = getInteger(stepJ, "step", 0);
                    std::string path = "patterns." + std::string(key) + ".steps[" + std::to_string(idx) + "]";
                    
                    if (e.step < 0 || e.step >= p.length) addError(res, path, "Step index out of bounds: " + std::to_string(e.step));
                    if (seenSteps.count(e.step)) addError(res, path, "Duplicate step index: " + std::to_string(e.step));
                    seenSteps.insert(e.step);
                    
                    json_t* pitchVJ = json_object_get(stepJ, "pitchV");
                    json_t* degreeJ = json_object_get(stepJ, "degree");
                    json_t* noteJ = json_object_get(stepJ, "note");
                    
                    int pitchDefs = (pitchVJ ? 1 : 0) + (degreeJ ? 1 : 0) + (noteJ ? 1 : 0);
                    if (pitchDefs != 1) {
                        addError(res, path, "Exactly one of pitchV, degree, or note is required");
                    } else {
                        if (pitchVJ) { e.pitchType = PitchType::PITCH_V; e.pitchV = json_number_value(pitchVJ); e.compiledPitchV = e.pitchV; }
                        else if (degreeJ) { e.pitchType = PitchType::DEGREE; e.degree = json_integer_value(degreeJ); e.octave = getInteger(stepJ, "octave", 0); }
                        else if (noteJ) { e.pitchType = PitchType::NOTE; e.note = json_string_value(noteJ); e.compiledPitchV = noteToPitchV(e.note, res, path); }
                    }
                    
                    json_t* gateJ = json_object_get(stepJ, "gate");
                    if (gateJ) { e.hasGate = true; e.gate = json_number_value(gateJ); }
                    json_t* velJ = json_object_get(stepJ, "velocity");
                    if (velJ) { e.hasVelocity = true; e.velocity = json_number_value(velJ); }
                    json_t* modJ = json_object_get(stepJ, "mod");
                    if (modJ) { e.hasMod = true; e.mod = json_number_value(modJ); }
                    json_t* probJ = json_object_get(stepJ, "probability");
                    if (probJ) { e.hasProbability = true; e.probability = json_number_value(probJ); }
                    
                    e.tie = getBoolean(stepJ, "tie", false);
                    e.glideMs = getNumber(stepJ, "glideMs", 0.0f);
                    e.microshift = getNumber(stepJ, "microshift", 0.0f);
                    e.ratchets = getInteger(stepJ, "ratchets", 1);
                    
                    p.steps.push_back(e);
                }
            }
            comp.patterns[p.id] = p;
        }
    }

    // Parse Arrangement
    json_t* arrJ = json_object_get(root, "arrangement");
    if (arrJ && json_is_array(arrJ)) {
        size_t idx; json_t* val;
        json_array_foreach(arrJ, idx, val) {
            Scene s;
            s.id = getString(val, "id");
            s.name = getString(val, "name");
            s.lengthBeats = getNumber(val, "lengthBeats", 16.0f);
            s.repeats = getInteger(val, "repeats", 1);
            s.phaseMode = parsePhaseMode(getString(val, "phaseMode", "restart"));
            
            json_t* tracksMapJ = json_object_get(val, "tracks");
            if (tracksMapJ && json_is_object(tracksMapJ)) {
                const char* tk; json_t* tv;
                json_object_foreach(tracksMapJ, tk, tv) {
                    TrackAssignment ta;
                    if (json_is_string(tv)) {
                        ta.patternId = json_string_value(tv);
                    } else if (json_is_object(tv)) {
                        ta.patternId = getString(tv, "pattern");
                        json_t* pmJ = json_object_get(tv, "phaseMode");
                        if (pmJ) {
                            ta.hasPhaseModeOverride = true;
                            ta.phaseModeOverride = parsePhaseMode(json_string_value(pmJ));
                        }
                    } else if (json_is_null(tv)) {
                        ta.patternId = ""; // rest
                    } else {
                        addError(res, "arrangement[" + std::to_string(idx) + "].tracks." + tk, "Invalid track assignment");
                    }
                    if (!ta.patternId.empty() && comp.patterns.find(ta.patternId) == comp.patterns.end()) {
                        addError(res, "arrangement[" + std::to_string(idx) + "].tracks." + tk, "Undefined pattern referenced: " + ta.patternId);
                    }
                    s.tracks[tk] = ta;
                }
            }
            comp.arrangement.push_back(s);
        }
    }

    json_decref(root);
    
    // Validate bounds
    if (comp.tracks.size() > 16) addError(res, "tracks", "Maximum 16 tracks allowed");
    if (comp.patterns.size() > 256) addError(res, "patterns", "Maximum 256 patterns allowed");
    if (comp.arrangement.size() > 256) addError(res, "arrangement", "Maximum 256 scenes allowed");
    if (comp.meta.bpm < 20.0f || comp.meta.bpm > 400.0f) addError(res, "meta.bpm", "BPM out of range 20-400");
    
    // Needs scale resolution for 'degree' pitch Types. Left out for MVP brevity but should be done here.

    if (!res.errors.empty()) res.valid = false;
    return res;
}

} // namespace sibyl
