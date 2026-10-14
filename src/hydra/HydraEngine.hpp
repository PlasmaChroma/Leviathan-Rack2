#pragma once

// Rack-independent Hydra engine: wraps the Beads granular processor and
// wavetable synth (MIT, Emilie Gillet / Mutable Instruments, see
// src/hydra/stmlib/LICENSE and doc/hydra_notes.md) behind a per-sample
// interface at the host sample rate. The Beads types are kept out of this
// header so Rack code never sees stmlib macros.

#include <cstddef>
#include <cstdint>
#include <memory>

namespace hydra {

// Quality order matches the front-panel LEDs (bright digital first).
enum Quality {
	QUALITY_BRIGHT_DIGITAL = 0,
	QUALITY_COLD_DIGITAL = 1,
	QUALITY_SUNNY_TAPE = 2,
	QUALITY_SCORCHED_CASSETTE = 3,
	QUALITY_LAST
};

enum SeedState {
	SEED_OFF = 0,
	SEED_ON = 1,
	SEED_LATCHED = 2
};

// Raw control values. Knobs are 0..1 pot positions (the engine applies the
// hardware pot laws); CVs are volts.
struct Controls {
	float time = 0.5f;
	float size = 0.5f;
	float shape = 0.5f;
	float pitch = 0.5f;
	float timeAmount = 0.5f;
	float sizeAmount = 0.5f;
	float shapeAmount = 0.5f;
	float pitchAmount = 0.5f;
	float density = 0.5f;
	float feedback = 0.f;
	float dryWet = 0.5f;
	float reverb = 0.f;

	float timeCv = 0.f;
	float sizeCv = 0.f;
	float shapeCv = 0.f;
	float pitchCv = 0.f;
	float densityCv = 0.f;
	float assignCv = 0.f;
	bool timeCvPatched = false;
	bool sizeCvPatched = false;
	bool shapeCvPatched = false;
	bool pitchCvPatched = false;
	bool densityCvPatched = false;

	// Amount of assign-CV applied to feedback, dry/wet, reverb (0..1 each).
	float assignAmount[3] = {0.f, 0.f, 0.f};

	bool seedInputPatched = false;
	int seedState = SEED_LATCHED;
	int quality = QUALITY_BRIGHT_DIGITAL;
	bool mono = false;
	bool gateOutput = false;
	bool synthMode = false;
};

class Engine {
public:
	Engine();
	~Engine();

	void init(float hostSampleRate);
	void setHostSampleRate(float hostSampleRate);
	void setControls(const Controls& controls);

	// Audio is +-1.0 full scale. Gates are sampled per call.
	void process(float inL, float inR, bool freeze, bool seedGate, float* outL, float* outR);

	// 0..1, how much of the record buffer remains before overwrite begins.
	float freezeReserve() const;
	float mediaSampleRate() const;
	bool inSynthMode() const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl;
};

} // namespace hydra
