#pragma once

#include <array>
#include <cstdint>

namespace doorstop {

constexpr int MODE_COUNT = 4;

enum class SoundModel : std::uint8_t {
	Classic = 0,
	CoupledBody,
	CoilContact,
	DispersiveSpring,
	ProbabilisticMix,
	Count
};

struct Tuning {
	float baseFrequencyHz = 16.f;
	float dampingRatio = 0.020f;
	float nonlinearStiffness = 1.4f;
	float maxImpulse = 150.f;
	float maximumStrikeImpulseBoost = 0.75f;
	float maximumStrikeModalBoost = 0.55f;
	float maximumStrikeImpactBoost = 0.40f;
	float maxDisplacement = 2.f;
	float maxVelocityInBaseOmega = 4.f;
	float energyKneeFraction = 0.75f;

	std::array<float, MODE_COUNT> modeFrequenciesHz {{155.f, 390.f, 820.f, 1650.f}};
	std::array<float, MODE_COUNT> modeDecayT60Seconds {{1.15f, 0.70f, 0.34f, 0.16f}};
	std::array<float, MODE_COUNT> modeExcitation {{220.f, 330.f, 520.f, 880.f}};
	std::array<float, MODE_COUNT> modeOutputGain {{1.20f, 0.90f, 0.55f, 0.27f}};
	std::array<float, MODE_COUNT> classicModeDecayScale {{0.62f, 0.60f, 0.68f, 0.75f}};
	std::array<float, MODE_COUNT> classicModeOutputScale {{0.62f, 0.68f, 0.78f, 0.82f}};
	std::array<float, MODE_COUNT> modeWarp {{0.030f, 0.024f, 0.018f, 0.012f}};
	std::array<float, MODE_COUNT> modeAsymmetry {{0.010f, -0.008f, 0.006f, -0.004f}};
	std::array<float, MODE_COUNT> modeCoupling {{420.f, 260.f, 0.f, 0.f}};
	std::array<float, MODE_COUNT> coupledStrikeScale {{0.72f, 0.48f, 0.24f, 0.10f}};
	std::array<float, MODE_COUNT> coupledMotionDrive {{980.f, 680.f, 390.f, 180.f}};
	std::array<float, MODE_COUNT - 1> coupledNeighborAmount {{0.010f, 0.007f, 0.004f}};
	float coupledSpringFeedback = 0.045f;
	float coupledWarpScale = 3.2f;
	float contactDisplacementThreshold = 0.34f;
	float contactEnergyThreshold = 0.03f;
	float contactMinimumRateHz = 600.f;
	float contactMaximumRateHz = 2400.f;
	float contactFastDecaySeconds = 0.00022f;
	float contactBodyDecaySeconds = 0.0016f;
	float contactRingDecaySeconds = 0.35f;
	float contactOutputGain = 0.75f;
	std::array<float, MODE_COUNT> contactModeExcitation {{0.f, 35.f, 240.f, 620.f}};
	std::array<float, MODE_COUNT> contactModeDecayScale {{1.f, 1.10f, 1.50f, 2.10f}};
	float waveguideRoundTripHz = 38.f;
	float waveguideFeedback = 0.86f;
	float waveguideSoftCutoffHz = 1600.f;
	float waveguideHardCutoffHz = 6000.f;
	float waveguideBrightnessDecaySeconds = 0.30f;
	float waveguideOutputGain = 0.18f;
	std::array<float, 3> waveguideAllpass {{0.38f, 0.57f, 0.71f}};
	float modalDriveDecaySeconds = 0.40f;

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
	float softImpactCutoffHz = 2400.f;
	float hardImpactCutoffHz = 8500.f;
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
	void setSoundModel(SoundModel newModel);
	void strike(float normalizedVelocity);
	Frame process(float sampleTime);

	bool isSleeping() const { return sleeping; }
	float getSampleRate() const { return sampleRate; }
	SoundModel getSoundModel() const { return soundModel; }
	SoundModel getLastStrikeModel() const { return lastStrikeModel; }
	const Tuning& getTuning() const { return tuning; }
	Tuning& getTuning() { return tuning; }

	static float shapeMagnitude(float normalizedMagnitude);
	static float manualVelocityFromVerticalPosition(float normalizedYFromTop);

private:
	static constexpr int MAX_WAVEGUIDE_DELAY = 8192;

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
	SoundModel soundModel = SoundModel::ProbabilisticMix;
	SoundModel lastStrikeModel = SoundModel::Classic;
	std::uint32_t modelRngState = 0x6d2b79f5u;
	float sampleRate = 44100.f;
	float sampleTime = 1.f / 44100.f;
	float baseOmega = 0.f;
	float baseOmegaSq = 0.f;
	float springDamping = 0.f;
	float thumpOmegaSq = 0.f;
	float maximumModeOmega = 2.f * 3.14159265358979323846f * 8820.f;
	float energyCeiling = 1.f;
	float energyKnee = 0.75f;
	float maxVelocity = 1.f;

	float displacement = 0.f;
	float springVelocity = 0.f;
	float acceleration = 0.f;
	static constexpr int MODAL_MODEL_COUNT = 3;
	std::array<std::array<Mode, MODE_COUNT>, MODAL_MODEL_COUNT> modalBanks {};
	std::array<float, MODAL_MODEL_COUNT> modalActivity {};
	std::array<bool, MODAL_MODEL_COUNT> modalBankActive {};
	Impact impact;

	float strikeLightEnvelope = 0.f;
	float quietTime = 0.f;
	float dcPreviousInput = 0.f;
	float dcPreviousOutput = 0.f;
	float contactPhase = 0.f;
	float contactSignal = 0.f;
	float contactBodySignal = 0.f;
	float contactRingEnvelope = 0.f;
	std::array<float, MAX_WAVEGUIDE_DELAY> waveguideDelay {};
	std::array<float, 3> waveguideAllpassState {};
	int waveguideWriteIndex = 0;
	int waveguideDelaySamples = 1024;
	float waveguideExcitation = 0.f;
	float waveguideLowpass = 0.f;
	float waveguidePreviousOutput = 0.f;
	float waveguideBrightness = 0.f;
	float waveguideActivity = 0.f;
	bool waveguideActive = false;
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
	float contactDecay = 0.f;
	float contactBodyDecay = 0.f;
	float contactRingDecay = 0.f;
	float waveguideSoftAlpha = 0.f;
	float waveguideHardAlpha = 0.f;
	float waveguideBrightnessDecay = 0.f;
	float waveguideActivityDecay = 0.f;
	float modalActivityDecay = 0.f;
	std::array<float, MODE_COUNT> baseModeOmega {};
	std::array<float, MODE_COUNT> modeEnergyScale {};
	std::array<float, MODE_COUNT> baseModeGamma {};
	std::array<float, MODE_COUNT> classicModeGamma {};

	void updateCoefficients();
	void clearDynamicState();
	void recoverFromNonFinite();
	float springPotential(float x) const;
	float primaryEnergy() const;
	float normalizedPrimaryEnergy() const;
	float normalizedModeEnergy(const Mode& mode, int index) const;
	float limitCandidateVelocity(float candidateVelocity) const;
	float processSpring();
	float processCoilContact();
	float processDispersiveSpring();
	float processModes();
	float processImpact();
	float processDcBlocker(float input);
	bool allFinite() const;
	bool belowSleepThreshold(float outputVolts) const;
	SoundModel chooseStrikeModel();
	std::uint32_t nextModelRandom();
	std::uint32_t nextRandom();
};

} // namespace doorstop
