#pragma once

#include <array>
#include <cstdint>

namespace doorstop {

constexpr int MODE_COUNT = 4;

struct Tuning {
	float baseFrequencyHz = 16.f;
	float dampingRatio = 0.020f;
	float nonlinearStiffness = 1.4f;
	float maxImpulse = 150.f;
	float maxDisplacement = 2.f;
	float maxVelocityInBaseOmega = 4.f;
	float energyKneeFraction = 0.75f;

	std::array<float, MODE_COUNT> modeFrequenciesHz {{190.f, 504.f, 1092.f, 2356.f}};
	std::array<float, MODE_COUNT> modeDecayT60Seconds {{1.15f, 0.70f, 0.34f, 0.16f}};
	std::array<float, MODE_COUNT> modeExcitation {{220.f, 330.f, 520.f, 880.f}};
	std::array<float, MODE_COUNT> modeOutputGain {{1.20f, 0.90f, 0.64f, 0.38f}};
	std::array<float, MODE_COUNT> modeWarp {{0.030f, 0.024f, 0.018f, 0.012f}};
	std::array<float, MODE_COUNT> modeAsymmetry {{0.010f, -0.008f, 0.006f, -0.004f}};
	std::array<float, MODE_COUNT> modeCoupling {{420.f, 260.f, 0.f, 0.f}};

	float accelerationScale = 24000.f;
	float bodyVelocityGain = 0.24f;
	float bodyAccelerationGain = 0.11f;
	float bodyDrive = 1.55f;
	float bodyGain = 0.72f;
	float modalGain = 0.82f;
	float impactGain = 0.72f;
	float outputDrive = 6.0f;

	float softImpactDecaySeconds = 0.003f;
	float hardImpactDecaySeconds = 0.012f;
	float softImpactCutoffHz = 2800.f;
	float hardImpactCutoffHz = 12000.f;
	float thumpFrequencyHz = 92.f;
	float thumpDecayT60Seconds = 0.11f;
	float strikeLightDecaySeconds = 0.075f;
	float dcBlockerCutoffHz = 10.f;

	float sleepEnergyThreshold = 1e-8f;
	float sleepEnvelopeThreshold = 1e-5f;
	float sleepOutputVoltsThreshold = 1e-4f;
	float sleepHoldSeconds = 0.050f;
};

struct Frame {
	float outputVolts = 0.f;
	float displacement = 0.f;
	float velocity = 0.f;
	float energy = 0.f;
	float strikeLight = 0.f;
	bool sleeping = true;
	bool enteredSleep = false;
};

class Engine {
public:
	Engine();

	void reset();
	void setSampleRate(float newSampleRate);
	void strike(float normalizedVelocity);
	Frame process(float sampleTime);

	bool isSleeping() const { return sleeping; }
	float getSampleRate() const { return sampleRate; }
	const Tuning& getTuning() const { return tuning; }
	Tuning& getTuning() { return tuning; }

	static float shapeMagnitude(float normalizedMagnitude);

private:
	struct Mode {
		float position = 0.f;
		float velocity = 0.f;
	};

	struct Impact {
		float noiseEnvelope = 0.f;
		float noiseBrightness = 0.f;
		float noiseLowpass = 0.f;
		float noiseLowReject = 0.f;
		float thumpPosition = 0.f;
		float thumpVelocity = 0.f;
		std::uint32_t rngState = 0x12345678u;
	};

	Tuning tuning;
	float sampleRate = 44100.f;
	float sampleTime = 1.f / 44100.f;
	float baseOmega = 0.f;
	float baseOmegaSq = 0.f;
	float maximumModeFrequency = 8820.f;
	float energyCeiling = 1.f;
	float energyKnee = 0.75f;
	float maxVelocity = 1.f;

	float displacement = 0.f;
	float springVelocity = 0.f;
	float acceleration = 0.f;
	std::array<Mode, MODE_COUNT> modes {};
	Impact impact;

	float strikeLightEnvelope = 0.f;
	float quietTime = 0.f;
	float dcPreviousInput = 0.f;
	float dcPreviousOutput = 0.f;
	bool sleeping = true;

	float softNoiseDecay = 0.f;
	float hardNoiseDecay = 0.f;
	float brightnessDecay = 0.f;
	float thumpDecayGamma = 0.f;
	float lightDecay = 0.f;
	float dcPole = 0.f;
	float softNoiseAlpha = 0.f;
	float hardNoiseAlpha = 0.f;
	float noiseRejectAlpha = 0.f;

	void updateCoefficients();
	void clearDynamicState();
	void recoverFromNonFinite();
	float springPotential(float x) const;
	float primaryEnergy() const;
	float normalizedPrimaryEnergy() const;
	float normalizedModeEnergy(int index, float frequencyHz) const;
	float limitCandidateVelocity(float candidateVelocity) const;
	float processSpring();
	float processModes();
	float processImpact();
	float processDcBlocker(float input);
	bool allFinite() const;
	bool belowSleepThreshold(float outputVolts) const;
	std::uint32_t nextRandom();
};

} // namespace doorstop
