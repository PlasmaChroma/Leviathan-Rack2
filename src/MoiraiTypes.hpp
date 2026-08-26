#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace moirai {

constexpr int kLaneCount = 2;
constexpr int kMaxChannels = 16;
constexpr int kMaxPrograms = 32;
constexpr int kMaxContourPointsPerProgram = 256;
constexpr int kMaxContourPointsPerBank = 8192;
constexpr int kMaxPathSegments = 32;
constexpr int kMaxMacroBindings = 16;
constexpr int kMaxVariants = 32;
constexpr int kMaxTransitionsPerSample = 64;
constexpr std::size_t kMaxDocumentBytes = 1024u * 1024u;

enum class DurationUnit { SECONDS, BEATS };
enum class ProgramKind { STAGED, CONTOUR };
enum class ProgramMode { GATE, ONE_SHOT, CYCLE };
enum class CurveType { LINEAR, SMOOTHSTEP, SIGMOID, HOLD, STEP, EXPONENTIAL, LOGARITHMIC };
enum class Interpolation { LINEAR, MONOTONE_CUBIC };
enum class RetriggerPolicy { RESTART, FROM_CURRENT, LEGATO, IGNORE_WHILE_RUNNING };
enum class LoopMode { NONE, COUNTED, WHILE_GATE };
enum class OutputMode { UNIPOLAR_10, UNIPOLAR_5, BIPOLAR_5 };
enum class EocSource { PROGRAM, LOOP };
enum class MacroSource { VELOCITY, M1, M2, M3 };
enum class MacroTarget { TIME_SCALE, CURVE_BIAS, LEVEL_SCALE, LEVEL_OFFSET, VARIANT_SELECT };
enum class MacroMapping { LINEAR, EXPONENTIAL };
enum class MacroSampling { ON_TRIGGER, CONTINUOUS };
enum class ClockLossPolicy { HOLD_TEMPO, FALLBACK };
enum class ApplyAt { IMMEDIATE, NEXT_TRIGGER, ALL_IDLE, NEXT_CLOCK };
enum class ActiveVoicePolicy { FINISH_CURRENT, RESTART_ACTIVE };

struct Duration {
	DurationUnit unit = DurationUnit::SECONDS;
	// Seconds for SECONDS, beats for BEATS.
	float value = 0.1f;
};

struct Curve {
	CurveType type = CurveType::LINEAR;
	float amount = 0.f;
};

struct LoopEnd {
	LoopMode mode = LoopMode::NONE;
	int count = 1;
};

struct Stage {
	std::string id;
	float target = 0.f;
	Duration duration;
	Curve curve;
	bool loopStart = false;
	LoopEnd loopEnd;
};

struct ContourPoint {
	float time = 0.f;
	float value = 0.f;
	ContourPoint() = default;
	ContourPoint(float time, float value) : time(time), value(value) {}
};

struct Variation {
	float level = 0.f;
	float time = 0.f;
};

struct MacroBinding {
	MacroSource source = MacroSource::M1;
	MacroTarget target = MacroTarget::LEVEL_SCALE;
	float inputMin = -10.f;
	float inputMax = 10.f;
	float outputMin = 0.f;
	float outputMax = 1.f;
	MacroMapping mapping = MacroMapping::LINEAR;
	MacroSampling sampling = MacroSampling::ON_TRIGGER;
	float smoothingMs = 5.f;
	std::vector<std::string> variants;
};

struct Program {
	std::string id;
	std::string name;
	ProgramKind kind = ProgramKind::STAGED;
	ProgramMode mode = ProgramMode::GATE;
	std::vector<Stage> gatePath;
	std::vector<Stage> releasePath;
	bool sustainHold = true;
	Duration duration;
	std::vector<ContourPoint> points;
	Interpolation interpolation = Interpolation::MONOTONE_CUBIC;
	RetriggerPolicy retrigger = RetriggerPolicy::RESTART;
	Variation variation;
	std::vector<MacroBinding> macroBindings;
	bool velocityDefault = true;
};

struct Lane {
	std::string defaultProgram;
	OutputMode outputMode = OutputMode::UNIPOLAR_10;
	EocSource eocSource = EocSource::PROGRAM;
	std::array<std::string, kMaxChannels> assignments {};
	std::array<std::string, kMaxChannels> channelLabels {};
};

struct ClockSettings {
	int externalPpqn = 4;
	float fallbackBpm = 120.f;
	float lossTimeoutMs = 2000.f;
	ClockLossPolicy onClockLoss = ClockLossPolicy::HOLD_TEMPO;
};

struct Bank {
	int schemaVersion = 1;
	int revision = 0;
	uint64_t seed = 0;
	ClockSettings clock;
	std::array<Lane, kLaneCount> lanes;
	std::unordered_map<std::string, Program> programs;
};

struct CompiledStage {
	float target = 0.f;
	Duration duration;
	Curve curve;
};

struct CompiledContourPoint {
	float time = 0.f;
	float value = 0.f;
	// dv/dt, calculated once during compilation.
	float tangent = 0.f;
	CompiledContourPoint() = default;
	CompiledContourPoint(float time, float value, float tangent)
		: time(time), value(value), tangent(tangent) {}
};

struct CompiledMacroBinding {
	MacroSource source = MacroSource::M1;
	MacroTarget target = MacroTarget::LEVEL_SCALE;
	float inputMin = -10.f;
	float inverseInputSpan = 0.05f;
	float outputMin = 0.f;
	float outputMax = 1.f;
	MacroMapping mapping = MacroMapping::LINEAR;
	MacroSampling sampling = MacroSampling::ON_TRIGGER;
	float smoothingMs = 5.f;
	std::vector<int> variantProgramIndices;
};

struct CompiledProgram {
	std::string id;
	std::string name;
	uint32_t stableIdHash = 0;
	ProgramKind kind = ProgramKind::STAGED;
	ProgramMode mode = ProgramMode::GATE;
	std::vector<CompiledStage> gatePath;
	std::vector<CompiledStage> releasePath;
	bool sustainHold = true;
	int loopStart = -1;
	int loopEnd = -1;
	LoopMode loopMode = LoopMode::NONE;
	int loopCount = 1;
	Duration duration;
	std::vector<CompiledContourPoint> points;
	Interpolation interpolation = Interpolation::MONOTONE_CUBIC;
	RetriggerPolicy retrigger = RetriggerPolicy::RESTART;
	Variation variation;
	std::vector<CompiledMacroBinding> macroBindings;
	float authoredPeak = 0.f;
	float gateDurationSeconds = 0.f;
	float gateDurationBeats = 0.f;
	float releaseDurationSeconds = 0.f;
	float releaseDurationBeats = 0.f;
};

struct CompiledLane {
	int defaultProgram = -1;
	OutputMode outputMode = OutputMode::UNIPOLAR_10;
	EocSource eocSource = EocSource::PROGRAM;
	std::array<int, kMaxChannels> assignments {};
	std::array<std::string, kMaxChannels> channelLabels {};

	CompiledLane() { assignments.fill(-1); }
};

struct CompiledBank {
	int revision = 0;
	uint64_t seed = 0;
	ClockSettings clock;
	std::vector<CompiledProgram> programs;
	std::unordered_map<std::string, int> programIndexById;
	std::array<CompiledLane, kLaneCount> lanes;
};

using CompiledBankPtr = std::shared_ptr<const CompiledBank>;

struct ValidationIssue {
	std::string code;
	std::string path;
	std::string message;
	ValidationIssue() = default;
	ValidationIssue(const std::string& code, const std::string& path, const std::string& message)
		: code(code), path(path), message(message) {}
};

struct CompileResult {
	bool valid = false;
	CompiledBankPtr bank;
	std::vector<ValidationIssue> errors;
	std::vector<ValidationIssue> warnings;
};

} // namespace moirai
