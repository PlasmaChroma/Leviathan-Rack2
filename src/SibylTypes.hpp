#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>

namespace sibyl {

enum class ScaleType {
	CHROMATIC, MAJOR, NATURAL_MINOR, HARMONIC_MINOR, MELODIC_MINOR,
	DORIAN, PHRYGIAN, LYDIAN, MIXOLYDIAN, LOCRIAN,
	MAJOR_PENTATONIC, MINOR_PENTATONIC
};

enum class OnExternalStop { HOLD, FREE_RUN, INTERNAL };

enum class ApplyAt { NEXT_BEAT, NEXT_STEP, NEXT_SCENE, IMMEDIATE };

enum class PhaseMode { RESTART, CONTINUE, ALIGN_GLOBAL };

enum class MacroPolarity { UNIPOLAR, BIPOLAR };

struct Meta {
	std::string title = "Untitled";
	std::string prompt;
	float bpm = 120.0f;
	std::string root = "C";
	int rootOctave = 4;
	ScaleType scale = ScaleType::CHROMATIC;
	float swing = 0.0f;
	uint64_t seed = 0;
};

struct Clock {
	int externalPpqn = 24;
	int outputPpqn = 24;
	float externalTimeoutMs = 2000.0f;
	OnExternalStop onExternalStop = OnExternalStop::HOLD;
};

struct Transport {
	bool running = true;
	bool loop = true;
	ApplyAt defaultApplyAt = ApplyAt::NEXT_BEAT;
};

struct TrackDef {
	std::string id;
	int channel = 0;
	float defaultGate = 0.5f;
	float defaultVelocity = 0.5f;
};

enum class PitchType { PITCH_V, DEGREE, NOTE };

struct StepEvent {
	int step = 0;
	PitchType pitchType = PitchType::PITCH_V;
	float pitchV = 0.0f;
	int degree = 0;
	std::string note;
	int octave = 0; // only valid with degree
	
	bool hasGate = false; float gate = 0.5f;
	bool hasVelocity = false; float velocity = 0.5f;
	bool hasMod = false; float mod = 0.0f;
	bool hasMod2 = false; float mod2 = 0.0f;
	bool hasMod3 = false; float mod3 = 0.0f;
	bool hasProbability = false; float probability = 1.0f;
	
	bool tie = false;
	float glideMs = 0.0f;
	float microshift = 0.0f;
	int ratchets = 1;

	// Compiled data (populated by compiler)
	float compiledPitchV = 0.0f; 
};

struct Pattern {
	std::string id;
	int length = 16;
	std::string resolutionStr = "1/16";
	// rational tick representation (e.g. 1/16 = 1 beat / 4)
	double resolutionBeats = 0.25; 
	std::vector<StepEvent> steps;
	// Compiled O(1) sparse-event lookup for the realtime scheduler. Entries are
	// indices into steps, or -1 for rests.
	std::vector<int> eventIndexByStep;
};

struct TrackAssignment {
	std::string patternId;
	bool hasPhaseModeOverride = false;
	PhaseMode phaseModeOverride = PhaseMode::RESTART;
};

struct Scene {
	std::string id;
	std::string name;
	std::string description;
	float lengthBeats = 16.0f;
	int repeats = 1;
	PhaseMode phaseMode = PhaseMode::RESTART;
	std::unordered_map<std::string, TrackAssignment> tracks; // key: track id
};

struct Macro {
	std::string id;
	std::string target;
	float amount = 0.0f;
	MacroPolarity polarity = MacroPolarity::UNIPOLAR;
	float clampMin = 0.0f;
	float clampMax = 1.0f;
};

// Represents an immutable snapshot of the entire compiled composition
struct Composition {
	int revision = 0;
	Meta meta;
	Clock clock;
	Transport transport;
	
	std::vector<TrackDef> tracks;
	std::unordered_map<std::string, Pattern> patterns;
	std::vector<Scene> arrangement;
	std::unordered_map<std::string, Macro> macros;
};

using CompositionPtr = std::shared_ptr<const Composition>;

} // namespace sibyl
