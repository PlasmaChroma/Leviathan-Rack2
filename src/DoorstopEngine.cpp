#include "DoorstopEngine.hpp"
#include "FastMath.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace doorstop {
namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float LN_1000 = 6.907755278982137f;

float clampf(float value, float low, float high) {
	return std::max(low, std::min(value, high));
}

float lerpf(float a, float b, float t) {
	return a + (b - a) * t;
}

float safeExpDecay(float seconds, float dt) {
	return std::exp(-dt / std::max(seconds, 1e-5f));
}

} // namespace

Engine::Engine() {
	updateCoefficients();
	clearDynamicState();
}

float Engine::shapeMagnitude(float normalizedMagnitude) {
	const float u = clampf(normalizedMagnitude, 0.f, 1.f);
	return 0.2f * u + 0.8f * u * u;
}

float Engine::manualVelocityFromVerticalPosition(float normalizedYFromTop) {
	const float topStrength = 1.f;
	const float bottomStrength = 0.10f;
	const float y = clampf(normalizedYFromTop, 0.f, 1.f);
	return lerpf(topStrength, bottomStrength, y);
}

void Engine::setSampleRate(float newSampleRate) {
	if (!std::isfinite(newSampleRate) || newSampleRate < 1000.f) {
		return;
	}
	sampleRate = newSampleRate;
	sampleTime = 1.f / sampleRate;
	updateCoefficients();
}

void Engine::setSoundModel(SoundModel newModel) {
	if (newModel >= SoundModel::Count) {
		newModel = SoundModel::ProbabilisticMix;
	}
	soundModel = newModel;
}

void Engine::updateCoefficients() {
	baseOmega = 2.f * PI * std::max(tuning.baseFrequencyHz, 0.1f);
	baseOmegaSq = baseOmega * baseOmega;
	springDamping = 2.f * tuning.dampingRatio * baseOmega;
	const float thumpOmega = 2.f * PI * tuning.thumpFrequencyHz;
	thumpOmegaSq = thumpOmega * thumpOmega;
	maxVelocity = baseOmega * std::max(tuning.maxVelocityInBaseOmega, 0.1f);
	const float maximumModeFrequency = std::min(12000.f, 0.20f * sampleRate);
	maximumModeOmega = 2.f * PI * maximumModeFrequency;
	energyCeiling = std::max(springPotential(std::max(tuning.maxDisplacement, 0.1f)), 1e-4f);
	energyKnee = clampf(tuning.energyKneeFraction, 0.f, 0.99f) * energyCeiling;

	softNoiseDecay = safeExpDecay(tuning.softImpactDecaySeconds, sampleTime);
	hardNoiseDecay = safeExpDecay(tuning.hardImpactDecaySeconds, sampleTime);
	brightnessDecay = safeExpDecay(0.030f, sampleTime);
	lightDecay = safeExpDecay(tuning.strikeLightDecaySeconds, sampleTime);
	thumpDecayGamma = LN_1000 / std::max(tuning.thumpDecayT60Seconds, 1e-4f);
	dcPole = std::exp(-2.f * PI * std::max(tuning.dcBlockerCutoffHz, 0.1f) * sampleTime);

	const float maxCutoff = 0.45f * sampleRate;
	const float softCutoff = std::min(tuning.softImpactCutoffHz, maxCutoff);
	const float hardCutoff = std::min(tuning.hardImpactCutoffHz, maxCutoff);
	softNoiseAlpha = 1.f - std::exp(-2.f * PI * softCutoff * sampleTime);
	hardNoiseAlpha = 1.f - std::exp(-2.f * PI * hardCutoff * sampleTime);
	noiseRejectAlpha = 1.f - std::exp(-2.f * PI * 180.f * sampleTime);
	contactDecay = safeExpDecay(tuning.contactFastDecaySeconds, sampleTime);
	contactBodyDecay = safeExpDecay(tuning.contactBodyDecaySeconds, sampleTime);
	contactRingDecay = safeExpDecay(tuning.contactRingDecaySeconds, sampleTime);
	waveguideDelaySamples = std::max(16, std::min(
		MAX_WAVEGUIDE_DELAY - 1,
		int(sampleRate / std::max(tuning.waveguideRoundTripHz, 1.f) + 0.5f)));
	const float waveguideMaxCutoff = 0.45f * sampleRate;
	waveguideSoftAlpha = 1.f - std::exp(-2.f * PI
		* std::min(tuning.waveguideSoftCutoffHz, waveguideMaxCutoff) * sampleTime);
	waveguideHardAlpha = 1.f - std::exp(-2.f * PI
		* std::min(tuning.waveguideHardCutoffHz, waveguideMaxCutoff) * sampleTime);
	waveguideBrightnessDecay = safeExpDecay(
		tuning.waveguideBrightnessDecaySeconds, sampleTime);
	waveguideActivityDecay = std::pow(
		clampf(tuning.waveguideFeedback, 0.01f, 0.999f),
		1.f / float(waveguideDelaySamples));
	modalActivityDecay = safeExpDecay(tuning.modalDriveDecaySeconds, sampleTime);
	for (int i = 0; i < MODE_COUNT; ++i) {
		baseModeOmega[i] = 2.f * PI * tuning.modeFrequenciesHz[i];
		const float referenceVelocity = std::max(tuning.modeExcitation[i], 1.f);
		modeEnergyScale[i] = 2.f / (referenceVelocity * referenceVelocity);
		baseModeGamma[i] = LN_1000
			/ std::max(tuning.modeDecayT60Seconds[i], 1e-4f);
		classicModeGamma[i] = LN_1000
			/ std::max(
				tuning.modeDecayT60Seconds[i] * tuning.classicModeDecayScale[i],
				1e-4f);
	}
}

void Engine::clearDynamicState() {
	displacement = 0.f;
	springVelocity = 0.f;
	acceleration = 0.f;
	modalBanks = {};
	modalActivity = {};
	modalBankActive = {};
	impact.noiseEnvelope = 0.f;
	impact.noiseBrightness = 0.f;
	impact.noiseLowpass = 0.f;
	impact.noiseLowReject = 0.f;
	impact.thumpPosition = 0.f;
	impact.thumpVelocity = 0.f;
	strikeLightEnvelope = 0.f;
	quietTime = 0.f;
	dcPreviousInput = 0.f;
	dcPreviousOutput = 0.f;
	contactPhase = 0.f;
	contactSignal = 0.f;
	contactBodySignal = 0.f;
	contactRingEnvelope = 0.f;
	waveguideDelay.fill(0.f);
	waveguideAllpassState.fill(0.f);
	waveguideWriteIndex = 0;
	waveguideExcitation = 0.f;
	waveguideLowpass = 0.f;
	waveguidePreviousOutput = 0.f;
	waveguideBrightness = 0.f;
	waveguideActivity = 0.f;
	waveguideActive = false;
}

void Engine::reset() {
	clearDynamicState();
	impact.rngState = 0x12345678u;
	modelRngState = 0x6d2b79f5u;
	lastStrikeModel = SoundModel::Classic;
	sleeping = true;
}

void Engine::recoverFromNonFinite() {
	clearDynamicState();
	sleeping = true;
}

float Engine::springPotential(float x) const {
	const float x2 = x * x;
	return 0.5f * baseOmegaSq * x2
		+ 0.25f * tuning.nonlinearStiffness * baseOmegaSq * x2 * x2;
}

float Engine::primaryEnergy() const {
	return 0.5f * springVelocity * springVelocity + springPotential(displacement);
}

float Engine::normalizedPrimaryEnergy() const {
	return primaryEnergy() / std::max(energyCeiling, 1e-6f);
}

float Engine::normalizedModeEnergy(const Mode& mode, int index) const {
	const float omega = baseModeOmega[index];
	const float energy = 0.5f * mode.velocity * mode.velocity
		+ 0.5f * omega * omega * mode.position * mode.position;
	return energy * modeEnergyScale[index];
}

std::uint32_t Engine::nextModelRandom() {
	std::uint32_t x = modelRngState;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	modelRngState = x ? x : 0x6d2b79f5u;
	return modelRngState;
}

SoundModel Engine::chooseStrikeModel() {
	if (soundModel != SoundModel::ProbabilisticMix) {
		return soundModel;
	}
	return static_cast<SoundModel>(nextModelRandom() % 4u);
}

float Engine::limitCandidateVelocity(float candidateVelocity) const {
	const float currentEnergy = primaryEnergy();
	const float candidateEnergy = 0.5f * candidateVelocity * candidateVelocity + springPotential(displacement);
	if (candidateEnergy <= currentEnergy || candidateEnergy <= energyKnee) {
		return clampf(candidateVelocity, -maxVelocity, maxVelocity);
	}

	const float span = std::max(energyCeiling - energyKnee, 1e-6f);
	const float compressed = energyKnee + span * std::tanh((candidateEnergy - energyKnee) / span);
	const float targetEnergy = std::max(currentEnergy, compressed);
	const float availableKinetic = std::max(0.f, targetEnergy - springPotential(displacement));
	const float targetSpeed = std::sqrt(2.f * availableKinetic);
	return std::copysign(std::min(targetSpeed, maxVelocity), candidateVelocity);
}

void Engine::strike(float normalizedVelocity) {
	if (!std::isfinite(normalizedVelocity)) {
		return;
	}
	const float clampedVelocity = clampf(normalizedVelocity, -1.f, 1.f);
	const float direction = std::copysign(1.f, clampedVelocity);
	const float shaped = shapeMagnitude(std::fabs(clampedVelocity));
	if (!(shaped > 0.f) || !std::isfinite(shaped)) {
		return;
	}
	const float signedShaped = direction * shaped;
	const SoundModel strikeModel = chooseStrikeModel();
	lastStrikeModel = strikeModel;
	// Reserve a dramatic extra force region for strikes near full velocity.
	// brightness^2 leaves ordinary and medium hits almost unchanged.
	const float maximumForceBlend = shaped * shaped * shaped * shaped;
	const float impulseBoost = 1.f
		+ tuning.maximumStrikeImpulseBoost * maximumForceBlend;
	const float modalBoost = 1.f
		+ tuning.maximumStrikeModalBoost * maximumForceBlend;
	const float impactBoost = 1.f
		+ tuning.maximumStrikeImpactBoost * maximumForceBlend;
	if (strikeModel == SoundModel::DispersiveSpring) {
		const float waveStrength = signedShaped
			* (0.65f + 0.75f * shaped * shaped) * impulseBoost;
		waveguideExcitation = clampf(waveguideExcitation + waveStrength, -2.f, 2.f);
		waveguideBrightness = std::max(waveguideBrightness, shaped);
		waveguideActivity = std::min(waveguideActivity + std::fabs(waveStrength), 2.f);
		waveguideActive = true;
	}

	sleeping = false;
	quietTime = 0.f;
	springVelocity = limitCandidateVelocity(
		springVelocity + signedShaped * tuning.maxImpulse * impulseBoost);

	const float brightness = shaped * shaped;
	if (strikeModel <= SoundModel::CoilContact) {
		const int bankIndex = int(strikeModel);
		auto& modes = modalBanks[bankIndex];
		modalBankActive[bankIndex] = true;
		modalActivity[bankIndex] = std::max(modalActivity[bankIndex], shaped);
		for (int i = 0; i < MODE_COUNT; ++i) {
			float excitation = shaped;
			if (i == 2) {
				excitation = lerpf(shaped, brightness, 0.6f);
			}
			else if (i == 3) {
				excitation = brightness;
			}
			const float strikeScale = strikeModel == SoundModel::Classic
				? 1.f
				: tuning.coupledStrikeScale[i];
			modes[i].velocity += direction * excitation * tuning.modeExcitation[i]
				* strikeScale * modalBoost;
			const float maxModeVelocity = tuning.modeExcitation[i] * 3.f;
			modes[i].velocity = clampf(
				modes[i].velocity, -maxModeVelocity, maxModeVelocity);
		}
	}

	impact.noiseEnvelope = std::min(impact.noiseEnvelope + shaped * impactBoost, 2.f);
	impact.noiseBrightness = std::max(impact.noiseBrightness, shaped);
	impact.thumpVelocity = clampf(
		impact.thumpVelocity + signedShaped * 48.f * impactBoost,
		-180.f,
		180.f);
	strikeLightEnvelope = std::max(strikeLightEnvelope, shaped);
}

float Engine::processSpring() {
	const float x2 = displacement * displacement;
	const float restoring = baseOmegaSq * displacement
		+ tuning.nonlinearStiffness * baseOmegaSq * displacement * x2;
	const float dampingForce = springDamping * springVelocity;
	acceleration = -restoring - dampingForce;
	float reaction = 0.f;
	for (int bankIndex = int(SoundModel::CoupledBody);
		bankIndex <= int(SoundModel::CoilContact); ++bankIndex) {
		if (!modalBankActive[bankIndex]) {
			continue;
		}
		const auto& modes = modalBanks[bankIndex];
		for (int i = 0; i < MODE_COUNT; ++i) {
			const float omega = baseModeOmega[i];
			reaction += modes[i].position * omega * omega
				* tuning.coupledStrikeScale[i];
		}
	}
	if (reaction != 0.f) {
		const float reactionLimit = 0.22f * std::max(tuning.accelerationScale, 1.f);
		acceleration += clampf(
			-reaction * tuning.coupledSpringFeedback,
			-reactionLimit,
			reactionLimit);
	}

	springVelocity += acceleration * sampleTime;
	displacement += springVelocity * sampleTime;

	if (displacement > tuning.maxDisplacement) {
		displacement = tuning.maxDisplacement;
		if (springVelocity > 0.f) springVelocity = 0.f;
	}
	else if (displacement < -tuning.maxDisplacement) {
		displacement = -tuning.maxDisplacement;
		if (springVelocity < 0.f) springVelocity = 0.f;
	}
	springVelocity = clampf(springVelocity, -maxVelocity, maxVelocity);

	const float normalizedVelocity = springVelocity / std::max(maxVelocity, 1.f);
	const float normalizedAcceleration = clampf(
		acceleration / std::max(tuning.accelerationScale, 1.f), -1.f, 1.f);
	return levi_math::tanhAudio((tuning.bodyVelocityGain * normalizedVelocity
		+ tuning.bodyAccelerationGain * normalizedAcceleration) * tuning.bodyDrive);
}

float Engine::processCoilContact() {
	const int contactBankIndex = int(SoundModel::CoilContact);
	if (modalActivity[contactBankIndex] > 1e-5f) {
		const float displacementRange = std::max(
			tuning.maxDisplacement - tuning.contactDisplacementThreshold, 1e-4f);
		const float compression = clampf(
			(std::fabs(displacement) - tuning.contactDisplacementThreshold)
				/ displacementRange,
			0.f,
			1.f);
		const float energyRange = std::max(1.f - tuning.contactEnergyThreshold, 1e-4f);
		const float energetic = clampf(
			(normalizedPrimaryEnergy() - tuning.contactEnergyThreshold) / energyRange,
			0.f,
			1.f);
		const float activity = compression * energetic;
		const float eventRate = lerpf(
			tuning.contactMinimumRateHz,
			tuning.contactMaximumRateHz,
			activity);
		contactPhase += eventRate * activity * sampleTime;
		if (contactPhase >= 1.f) {
			contactPhase -= std::floor(contactPhase);
			const float random = (float(nextRandom() & 0x00ffffffu)
				/ float(0x00800000u)) - 1.f;
			const float impulse = activity * (0.45f + 0.55f * std::fabs(random))
				* (random < 0.f ? -1.f : 1.f);
			contactSignal = clampf(contactSignal + impulse, -2.f, 2.f);
			contactBodySignal = clampf(contactBodySignal + impulse, -2.f, 2.f);
			contactRingEnvelope = std::max(
				contactRingEnvelope, clampf(activity * 2.5f, 0.f, 1.f));
			auto& modes = modalBanks[contactBankIndex];
			for (int i = 0; i < MODE_COUNT; ++i) {
				modes[i].velocity += impulse * tuning.contactModeExcitation[i];
				const float maxModeVelocity = tuning.modeExcitation[i] * 3.f;
				modes[i].velocity = clampf(
					modes[i].velocity, -maxModeVelocity, maxModeVelocity);
			}
		}
	}

	// Two unequal decays turn each collision into a short bipolar metal tick,
	// avoiding a DC-like pulse and preserving it through the final saturation.
	const float output = (contactSignal - contactBodySignal) * tuning.contactOutputGain;
	contactSignal *= contactDecay;
	contactBodySignal *= contactBodyDecay;
	contactRingEnvelope *= contactRingDecay;
	return output;
}

float Engine::processDispersiveSpring() {
	if (!waveguideActive) {
		return 0.f;
	}
	int readIndex = waveguideWriteIndex - waveguideDelaySamples;
	if (readIndex < 0) {
		readIndex += MAX_WAVEGUIDE_DELAY;
	}
	const float delayed = waveguideDelay[readIndex];
	const float brightness = clampf(waveguideBrightness, 0.f, 1.f);
	const float lowpassAlpha = lerpf(waveguideSoftAlpha, waveguideHardAlpha, brightness);
	waveguideLowpass += (delayed - waveguideLowpass) * lowpassAlpha;

	float dispersed = waveguideLowpass;
	for (int i = 0; i < int(waveguideAllpassState.size()); ++i) {
		const float coefficient = clampf(tuning.waveguideAllpass[i], -0.95f, 0.95f);
		const float output = -coefficient * dispersed + waveguideAllpassState[i];
		waveguideAllpassState[i] = dispersed + coefficient * output;
		dispersed = output;
	}

	const float feedback = waveguideActivity > tuning.sleepEnvelopeThreshold
		? clampf(tuning.waveguideFeedback, 0.f, 0.98f)
		: 0.72f;
	const float writeValue = clampf(
		waveguideExcitation - dispersed * feedback, -2.f, 2.f);
	waveguideDelay[waveguideWriteIndex] = writeValue;
	waveguideWriteIndex++;
	if (waveguideWriteIndex >= MAX_WAVEGUIDE_DELAY) {
		waveguideWriteIndex = 0;
	}
	waveguideExcitation = 0.f;
	waveguideBrightness *= waveguideBrightnessDecay;
	waveguideActivity *= waveguideActivityDecay;

	const float radiation = delayed - 0.25f * waveguidePreviousOutput;
	waveguidePreviousOutput = delayed;
	return radiation * tuning.waveguideOutputGain;
}

float Engine::processModes() {
	float output = 0.f;
	const float normalizedAcceleration = clampf(
		acceleration / std::max(tuning.accelerationScale, 1.f), -1.f, 1.f);
	for (int bankIndex = 0; bankIndex < MODAL_MODEL_COUNT; ++bankIndex) {
		if (!modalBankActive[bankIndex]) {
			continue;
		}
		const SoundModel model = static_cast<SoundModel>(bankIndex);
		auto& modes = modalBanks[bankIndex];
		const bool coupled = model != SoundModel::Classic;
		const float driveActivity = clampf(modalActivity[bankIndex], 0.f, 1.f);
		for (int i = 0; i < MODE_COUNT; ++i) {
			const float warpScale = coupled ? tuning.coupledWarpScale : 1.f;
			const float warp = 1.f
				+ tuning.modeWarp[i] * warpScale * displacement * displacement;
			const float asymmetry = 1.f + tuning.modeAsymmetry[i] * displacement;
			const float omega = clampf(
				baseModeOmega[i] * warp * asymmetry,
				2.f * PI * 20.f,
				maximumModeOmega);
			float gamma = baseModeGamma[i];
			if (model == SoundModel::Classic) {
				gamma = classicModeGamma[i];
			}
			else if (model == SoundModel::CoilContact
				&& contactRingEnvelope > tuning.sleepEnvelopeThreshold) {
				const float decayScale = lerpf(
					1.f, tuning.contactModeDecayScale[i], contactRingEnvelope);
				gamma = baseModeGamma[i] / std::max(decayScale, 1e-4f);
			}
			float force = normalizedAcceleration * driveActivity
				* (coupled ? tuning.coupledMotionDrive[i] : tuning.modeCoupling[i]);
			if (coupled) {
				if (i > 0) {
					const float neighborOmega = baseModeOmega[i - 1];
					force += modes[i - 1].position * omega * neighborOmega
						* tuning.coupledNeighborAmount[i - 1];
				}
				if (i + 1 < MODE_COUNT) {
					const float neighborOmega = baseModeOmega[i + 1];
					force += modes[i + 1].position * omega * neighborOmega
						* tuning.coupledNeighborAmount[i];
				}
			}
			Mode& mode = modes[i];
			const float modeAcceleration = -omega * omega * mode.position
				- 2.f * gamma * mode.velocity + force;
			mode.velocity += modeAcceleration * sampleTime;
			mode.position += mode.velocity * sampleTime;
			const float outputScale = model == SoundModel::Classic
				? tuning.classicModeOutputScale[i]
				: 1.f;
			output += mode.position * tuning.modeOutputGain[i] * outputScale;
		}
		modalActivity[bankIndex] *= modalActivityDecay;
		if (modalActivity[bankIndex] < tuning.sleepEnvelopeThreshold) {
			bool bankSettled = true;
			for (int i = 0; i < MODE_COUNT; ++i) {
				if (normalizedModeEnergy(modes[i], i)
					>= tuning.sleepEnergyThreshold) {
					bankSettled = false;
					break;
				}
			}
			if (bankSettled) {
				modes = {};
				modalActivity[bankIndex] = 0.f;
				modalBankActive[bankIndex] = false;
			}
		}
	}
	return output;
}

std::uint32_t Engine::nextRandom() {
	std::uint32_t x = impact.rngState;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	impact.rngState = x ? x : 0x12345678u;
	return impact.rngState;
}

float Engine::processImpact() {
	const float noise = (float(nextRandom() & 0x00ffffffu) / float(0x00800000u)) - 1.f;
	const float brightness = clampf(impact.noiseBrightness, 0.f, 1.f);
	const float noiseAlpha = lerpf(softNoiseAlpha, hardNoiseAlpha, brightness);
	impact.noiseLowpass += (noise - impact.noiseLowpass) * noiseAlpha;
	impact.noiseLowReject += (impact.noiseLowpass - impact.noiseLowReject) * noiseRejectAlpha;
	const float coloredNoise = impact.noiseLowpass - impact.noiseLowReject;
	const float strikeNoise = coloredNoise * impact.noiseEnvelope * (0.22f + 0.28f * brightness);

	const float envelopeDecay = lerpf(softNoiseDecay, hardNoiseDecay, brightness);
	impact.noiseEnvelope *= envelopeDecay;
	impact.noiseBrightness *= brightnessDecay;

	const float thumpAcceleration = -thumpOmegaSq * impact.thumpPosition
		- 2.f * thumpDecayGamma * impact.thumpVelocity;
	impact.thumpVelocity += thumpAcceleration * sampleTime;
	impact.thumpPosition += impact.thumpVelocity * sampleTime;
	const float thump = impact.thumpPosition * 0.70f;

	strikeLightEnvelope *= lightDecay;
	return strikeNoise + thump;
}

float Engine::processDcBlocker(float input) {
	const float output = input - dcPreviousInput + dcPole * dcPreviousOutput;
	dcPreviousInput = input;
	dcPreviousOutput = output;
	return output;
}

bool Engine::allFinite() const {
	if (!std::isfinite(displacement) || !std::isfinite(springVelocity)
		|| !std::isfinite(acceleration) || !std::isfinite(strikeLightEnvelope)
		|| !std::isfinite(dcPreviousInput) || !std::isfinite(dcPreviousOutput)
		|| !std::isfinite(impact.noiseEnvelope) || !std::isfinite(impact.noiseBrightness)
		|| !std::isfinite(impact.noiseLowpass) || !std::isfinite(impact.noiseLowReject)
		|| !std::isfinite(impact.thumpPosition) || !std::isfinite(impact.thumpVelocity)
		|| !std::isfinite(contactPhase) || !std::isfinite(contactSignal)
		|| !std::isfinite(contactBodySignal) || !std::isfinite(contactRingEnvelope)
		|| !std::isfinite(waveguideExcitation) || !std::isfinite(waveguideLowpass)
		|| !std::isfinite(waveguidePreviousOutput) || !std::isfinite(waveguideBrightness)
		|| !std::isfinite(waveguideActivity)) {
		return false;
	}
	for (int bankIndex = 0; bankIndex < MODAL_MODEL_COUNT; ++bankIndex) {
		if (!std::isfinite(modalActivity[bankIndex])) {
			return false;
		}
		if (!modalBankActive[bankIndex]) {
			continue;
		}
		for (const Mode& mode : modalBanks[bankIndex]) {
			if (!std::isfinite(mode.position) || !std::isfinite(mode.velocity)) {
				return false;
			}
		}
	}
	for (float state : waveguideAllpassState) {
		if (!std::isfinite(state)) {
			return false;
		}
	}
	return true;
}

bool Engine::belowSleepThreshold(float outputVolts) const {
	if (normalizedPrimaryEnergy() >= tuning.sleepEnergyThreshold) {
		return false;
	}
	for (int bankIndex = 0; bankIndex < MODAL_MODEL_COUNT; ++bankIndex) {
		if (!modalBankActive[bankIndex]) {
			continue;
		}
		const auto& bank = modalBanks[bankIndex];
		for (int i = 0; i < MODE_COUNT; ++i) {
			if (normalizedModeEnergy(bank[i], i)
				>= tuning.sleepEnergyThreshold) {
				return false;
			}
		}
	}
	return impact.noiseEnvelope < tuning.sleepEnvelopeThreshold
		&& std::fabs(impact.thumpPosition) < tuning.sleepEnvelopeThreshold
		&& std::fabs(impact.thumpVelocity) < tuning.sleepEnvelopeThreshold
		&& std::fabs(contactSignal) < tuning.sleepEnvelopeThreshold
		&& std::fabs(contactBodySignal) < tuning.sleepEnvelopeThreshold
		&& contactRingEnvelope < tuning.sleepEnvelopeThreshold
		&& waveguideActivity < tuning.sleepEnvelopeThreshold
		&& strikeLightEnvelope < tuning.sleepEnvelopeThreshold
		&& std::fabs(outputVolts) < tuning.sleepOutputVoltsThreshold;
}

Frame Engine::process(float requestedSampleTime) {
	if (requestedSampleTime != sampleTime
		&& std::isfinite(requestedSampleTime) && requestedSampleTime > 0.f) {
		const float requestedRate = 1.f / requestedSampleTime;
		if (std::fabs(requestedRate - sampleRate) > 1.f) {
			setSampleRate(requestedRate);
		}
	}

	if (sleeping) {
		return {};
	}

	const float body = processSpring();
	const float contact = processCoilContact();
	const float waveguide = processDispersiveSpring();
	const float modal = processModes();
	const float transient = processImpact();
	float signal = body * tuning.bodyGain + modal * tuning.modalGain
		+ transient * tuning.impactGain + contact + waveguide;
	signal = processDcBlocker(signal);
	const float outputVolts = 5.f * levi_math::tanhAudio(signal * tuning.outputDrive);

	if (!allFinite() || !std::isfinite(outputVolts)) {
		recoverFromNonFinite();
		return {};
	}

	Frame frame;
	frame.outputVolts = outputVolts;
	frame.displacement = displacement;
	frame.velocity = springVelocity / std::max(maxVelocity, 1.f);
	frame.energy = clampf(normalizedPrimaryEnergy(), 0.f, 1.f);
	frame.strikeLight = clampf(strikeLightEnvelope, 0.f, 1.f);
	frame.sleeping = false;

	if (belowSleepThreshold(outputVolts)) {
		quietTime += sampleTime;
	}
	else {
		quietTime = 0.f;
	}

	if (quietTime >= tuning.sleepHoldSeconds) {
		const float finalOutput = frame.outputVolts;
		clearDynamicState();
		sleeping = true;
		frame = {};
		frame.outputVolts = finalOutput;
		frame.sleeping = true;
		frame.enteredSleep = true;
	}

	return frame;
}

} // namespace doorstop
