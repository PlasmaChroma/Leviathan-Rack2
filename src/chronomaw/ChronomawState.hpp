#pragma once

#include <array>
#include <cstdint>

namespace chronomaw {

static constexpr int kNumOutputs = 8;
static constexpr int kNumBanks = 64;
static constexpr float kDefaultBpm = 120.0f;
static constexpr float kMinBpm = 10.0f;
static constexpr float kMaxBpm = 330.0f;
static constexpr float kOutputMinV = 0.0f;
static constexpr float kOutputMaxV = 5.0f;
static constexpr int kDefaultExternalPpqn = 24;

enum class ClockInputMode : int {
	Clock = 0,
	Cv = 1,
	NextBank = 2,
};

enum class RunInputMode : int {
	RunGate = 0,
	Reset = 1,
	Cv = 2,
	PrevBank = 3,
	Rotate = 4,
};

enum class DensityMode : int {
	Monitor = 0,
	Edit = 1,
	Focus = 2,
};

enum class WaveformMode : int {
	Gate = 0,
	RatchetX2 = 1,
	RatchetX4 = 2,
	Triangle = 3,
	Trapezoid = 4,
	Sine = 5,
	Hump = 6,
	ExpEnvelope = 7,
	LogEnvelope = 8,
	ClassicRandom = 9,
	SmoothRandom = 10,
};

enum class ModifierMode : int {
	Div = 0,
	Mult = 1,
	Util = 2,
};

struct OutputState {
	bool muted = false;
	WaveformMode waveform = WaveformMode::Gate;
	ModifierMode modifierMode = ModifierMode::Mult;
	float multiplier = 1.f;
	float widthPct = 50.f;
	float levelPct = 100.f;
	float offsetPct = 0.f;
	float phasePct = 0.f;
	float swingPct = 0.f;
	float skewPct = 0.f;
	float rotatePct = 0.f;
	float probabilityPct = 100.f;
	bool invert = false;
	uint32_t randomSeed = 0u;
};

struct LiveState {
	float bpm = kDefaultBpm;
	bool running = false;
	ClockInputMode clkMode = ClockInputMode::Clock;
	RunInputMode runMode = RunInputMode::RunGate;
	int extPpqn = kDefaultExternalPpqn;
	int activeBank = 0;
	DensityMode density = DensityMode::Monitor;
	std::array<OutputState, kNumOutputs> outputs {};
};

struct BankState {
	float bpm = kDefaultBpm;
	std::array<OutputState, kNumOutputs> outputs {};
};

struct UiState {
	int selectedOutput = 0;
	int selectedTab = 0;
	bool sampledFutureTimeline = false;
};

struct ModuleState {
	static constexpr int kSchemaVersion = 1;
	LiveState live;
	std::array<BankState, kNumBanks> banks {};
	UiState ui;
};

} // namespace chronomaw
