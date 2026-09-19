#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>
#include "SibylCondition.hpp"
#include "SibylTuning.hpp"
#include "SibylOverrides.hpp"
#include "SibylAutomation.hpp"
#include "SibylHarmonyTypes.hpp"

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

enum class PitchType { PITCH_V, DEGREE, NOTE, HARMONIC, TUNED };

struct ObservationMarker {
	int64_t octaviaModuleId = -1;
	uint32_t preFrames = 0;
	uint32_t postFrames = 0;
	uint8_t monitorMask = 0;
	std::string label;
};

// Symmetric maximum deviations from authored values; zero disables a lane.
struct RepeatEvolution {
	float probability = 0.f;
	float velocity = 0.f;
	float gate = 0.f; // pattern steps
	float glideMs = 0.f;
	float mod[3] {};
	bool enabled() const {
		return probability != 0.f || velocity != 0.f || gate != 0.f || glideMs != 0.f
			|| mod[0] != 0.f || mod[1] != 0.f || mod[2] != 0.f;
	}
	bool operator==(const RepeatEvolution& b) const {
		return probability == b.probability && velocity == b.velocity && gate == b.gate && glideMs == b.glideMs
			&& mod[0] == b.mod[0] && mod[1] == b.mod[1] && mod[2] == b.mod[2];
	}
};

struct StepEvent {
	std::string tuned;
	PitchOffsets pitchOffsets;
	NativePitch nativePitch;
	std::string harmonic;
	int harmonicExpression = -1;
	Condition condition;
	std::string id;
	int transposeSemitones = 0;
	int step = 0;
	bool evolve = true; // false protects this event from repeat evolution
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
	bool hasObservation = false;
	ObservationMarker observation;

	// Compiled data (populated by compiler)
	float compiledPitchV = 0.0f; 
};

struct Pattern {
	std::string pitchContext;
	int nextNoteId = 1;
	std::string id;
	int length = 16;
	std::string resolutionStr = "1/16";
	// rational tick representation (e.g. 1/16 = 1 beat / 4)
	double resolutionBeats = 0.25; 
	std::vector<StepEvent> steps;
	RepeatEvolution evolution;
	// Compiled O(1) sparse-event lookup for the realtime scheduler. Entries are
	// indices into steps, or -1 for rests.
	std::vector<int> eventIndexByStep;
};

struct TrackAssignment {
	std::shared_ptr<const std::vector<HarmonicRoutePitch>> harmonicPitches;
	std::shared_ptr<const std::vector<float>> compiledPitches; // Indexed by temporal step, allocated only for native transforms.
	AssignmentOverrides overrides;
	std::string patternId;
	bool hasPhaseModeOverride = false;
	PhaseMode phaseModeOverride = PhaseMode::RESTART;
	bool operator==(const TrackAssignment& b) const {
		return ((!harmonicPitches && !b.harmonicPitches) || (harmonicPitches && b.harmonicPitches && *harmonicPitches==*b.harmonicPitches)) && ((!compiledPitches && !b.compiledPitches) || (compiledPitches && b.compiledPitches && *compiledPitches==*b.compiledPitches)) && patternId == b.patternId && hasPhaseModeOverride == b.hasPhaseModeOverride &&
			phaseModeOverride == b.phaseModeOverride && overrides == b.overrides;
	}
};

struct Scene {
	HarmonyBinding harmony;
	std::string id;
	std::string name;
	std::string description;
	float lengthBeats = 16.0f;
	double authoredLengthBeats = 0.; // New timelines retain JSON double precision; legacy DSP stays float.
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

inline double sceneTimelineLength(const Scene& scene) {
	return scene.authoredLengthBeats > 0. && float(scene.authoredLengthBeats) == scene.lengthBeats
		? scene.authoredLengthBeats : double(scene.lengthBeats);
}

// Represents an immutable snapshot of the entire compiled composition
struct Composition {
	PitchSystems pitchSystems;
	size_t pitchStorageBytes = 0, staticPitchEntries = 0;
	int revision = 0;
	bool harmonyPresent = false;
	HarmonyBinding defaultHarmony;
	std::vector<HarmonyProgression> progressions;
	std::vector<HarmonicExpression> harmonicExpressions;
	size_t harmonicPitchEntries = 0;
	Meta meta;
	Clock clock;
	Transport transport;
	
	std::vector<TrackDef> tracks;
	std::unordered_map<std::string, Pattern> patterns;
	std::vector<Scene> arrangement;
	std::unordered_map<std::string, Macro> macros;
	std::vector<AutomationCurve> automation; // Stable ID order, including disabled definitions.
	std::vector<AutomationRoute> automationRoutes;
	std::vector<double> sceneBeatPrefixes;
	double arrangementDuration = 0.;
};

inline float assignedPitch(const StepEvent& event, const TrackAssignment& assignment, float eventPitch) {
    if (assignment.compiledPitches!=nullptr && event.pitchType!=PitchType::HARMONIC)
        return (*assignment.compiledPitches)[size_t(event.step)];
    float pitch=assignment.overrides.pitch(eventPitch);
    const auto& offset=assignment.overrides.pitchOffsets;
    if(offset.fields) pitch=float(double(pitch)+double(offset.periods)+offset.cents/1200.);
    return pitch;
}

using CompositionPtr = std::shared_ptr<const Composition>;

} // namespace sibyl
