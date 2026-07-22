#include "DoorstopEngine.hpp"

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

void Engine::setSampleRate(float newSampleRate) {
	if (!std::isfinite(newSampleRate) || newSampleRate < 1000.f) {
		return;
	}
	sampleRate = newSampleRate;
	sampleTime = 1.f / sampleRate;
	updateCoefficients();
}

void Engine::updateCoefficients() {
	baseOmega = 2.f * PI * std::max(tuning.baseFrequencyHz, 0.1f);
	baseOmegaSq = baseOmega * baseOmega;
	maxVelocity = baseOmega * std::max(tuning.maxVelocityInBaseOmega, 0.1f);
	maximumModeFrequency = std::min(12000.f, 0.20f * sampleRate);
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
}

void Engine::clearDynamicState() {
	displacement = 0.f;
	springVelocity = 0.f;
	acceleration = 0.f;
	modes = {};
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
}

void Engine::reset() {
	clearDynamicState();
	impact.rngState = 0x12345678u;
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

float Engine::normalizedModeEnergy(int index, float frequencyHz) const {
	const float omega = 2.f * PI * frequencyHz;
	const Mode& mode = modes[index];
	const float energy = 0.5f * mode.velocity * mode.velocity
		+ 0.5f * omega * omega * mode.position * mode.position;
	const float referenceVelocity = std::max(tuning.modeExcitation[index], 1.f);
	const float referenceEnergy = 0.5f * referenceVelocity * referenceVelocity;
	return energy / referenceEnergy;
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

	sleeping = false;
	quietTime = 0.f;
	springVelocity = limitCandidateVelocity(springVelocity + signedShaped * tuning.maxImpulse);

	const float brightness = shaped * shaped;
	for (int i = 0; i < MODE_COUNT; ++i) {
		float excitation = shaped;
		if (i == 2) {
			excitation = lerpf(shaped, brightness, 0.6f);
		}
		else if (i == 3) {
			excitation = brightness;
		}
		modes[i].velocity += direction * excitation * tuning.modeExcitation[i];
		const float maxModeVelocity = tuning.modeExcitation[i] * 3.f;
		modes[i].velocity = clampf(modes[i].velocity, -maxModeVelocity, maxModeVelocity);
	}

	impact.noiseEnvelope = std::min(impact.noiseEnvelope + shaped, 2.f);
	impact.noiseBrightness = std::max(impact.noiseBrightness, shaped);
	impact.thumpVelocity = clampf(
		impact.thumpVelocity + signedShaped * 48.f,
		-180.f,
		180.f);
	strikeLightEnvelope = std::max(strikeLightEnvelope, shaped);
}

float Engine::processSpring() {
	const float x2 = displacement * displacement;
	const float restoring = baseOmegaSq * displacement
		+ tuning.nonlinearStiffness * baseOmegaSq * displacement * x2;
	const float dampingForce = 2.f * tuning.dampingRatio * baseOmega * springVelocity;
	acceleration = -restoring - dampingForce;

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
	return std::tanh((tuning.bodyVelocityGain * normalizedVelocity
		+ tuning.bodyAccelerationGain * normalizedAcceleration) * tuning.bodyDrive);
}

float Engine::processModes() {
	float output = 0.f;
	const float normalizedAcceleration = clampf(
		acceleration / std::max(tuning.accelerationScale, 1.f), -1.f, 1.f);
	for (int i = 0; i < MODE_COUNT; ++i) {
		const float warp = 1.f + tuning.modeWarp[i] * displacement * displacement;
		const float asymmetry = 1.f + tuning.modeAsymmetry[i] * displacement;
		const float frequency = clampf(
			tuning.modeFrequenciesHz[i] * warp * asymmetry,
			20.f,
			maximumModeFrequency);
		const float omega = 2.f * PI * frequency;
		const float gamma = LN_1000 / std::max(tuning.modeDecayT60Seconds[i], 1e-4f);
		const float force = normalizedAcceleration * tuning.modeCoupling[i];
		Mode& mode = modes[i];
		const float modeAcceleration = -omega * omega * mode.position - 2.f * gamma * mode.velocity + force;
		mode.velocity += modeAcceleration * sampleTime;
		mode.position += mode.velocity * sampleTime;
		output += mode.position * tuning.modeOutputGain[i];
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

	const float thumpOmega = 2.f * PI * tuning.thumpFrequencyHz;
	const float thumpAcceleration = -thumpOmega * thumpOmega * impact.thumpPosition
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
		|| !std::isfinite(impact.thumpPosition) || !std::isfinite(impact.thumpVelocity)) {
		return false;
	}
	for (const Mode& mode : modes) {
		if (!std::isfinite(mode.position) || !std::isfinite(mode.velocity)) {
			return false;
		}
	}
	return true;
}

bool Engine::belowSleepThreshold(float outputVolts) const {
	if (normalizedPrimaryEnergy() >= tuning.sleepEnergyThreshold) {
		return false;
	}
	for (int i = 0; i < MODE_COUNT; ++i) {
		if (normalizedModeEnergy(i, tuning.modeFrequenciesHz[i]) >= tuning.sleepEnergyThreshold) {
			return false;
		}
	}
	return impact.noiseEnvelope < tuning.sleepEnvelopeThreshold
		&& std::fabs(impact.thumpPosition) < tuning.sleepEnvelopeThreshold
		&& std::fabs(impact.thumpVelocity) < tuning.sleepEnvelopeThreshold
		&& strikeLightEnvelope < tuning.sleepEnvelopeThreshold
		&& std::fabs(outputVolts) < tuning.sleepOutputVoltsThreshold;
}

Frame Engine::process(float requestedSampleTime) {
	if (std::isfinite(requestedSampleTime) && requestedSampleTime > 0.f) {
		const float requestedRate = 1.f / requestedSampleTime;
		if (std::fabs(requestedRate - sampleRate) > 1.f) {
			setSampleRate(requestedRate);
		}
	}

	if (sleeping) {
		return {};
	}

	const float body = processSpring();
	const float modal = processModes();
	const float transient = processImpact();
	float signal = body * tuning.bodyGain + modal * tuning.modalGain + transient * tuning.impactGain;
	signal = processDcBlocker(signal);
	const float outputVolts = 5.f * std::tanh(signal * tuning.outputDrive);

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
