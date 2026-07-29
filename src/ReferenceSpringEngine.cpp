#include "ReferenceSpringEngine.hpp"
#include "MathHelpers.hpp"

#include <algorithm>
#include <cmath>

namespace doorstop {
namespace {

constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_FREQUENCIES {{
	375.f, 586.f, 773.f, 1289.f, 1580.f, 2508.f, 3380.f, 4680.f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_DECAYS {{
	8.4f, 8.4f, 7.9f, 7.9f, 6.8f, 5.8f, 4.2f, 3.15f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_IMPACT {{
	235.f, 270.f, 285.f, 310.f, 275.f, 245.f, 155.f, 90.f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_CROSSING {{
	18.f, -14.f, 11.f, -8.f, 6.f, -4.f, 2.5f, -1.5f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_OUTPUT {{
	1.07f, 0.95f, 0.95f, 2.15f, 2.35f, 3.58f, 4.30f, 7.40f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_WARP_DEPTH {{
	0.008f, 0.020f, 0.038f, 0.070f, 0.060f, 0.040f, 0.025f, 0.015f
}};

float clampf(float value, float low, float high) {
	return std::max(low, std::min(value, high));
}

float lerpf(float a, float b, float t) {
	return a + (b - a) * t;
}

float safeDecay(float seconds, float sampleTime) {
	return std::exp(-sampleTime / std::max(seconds, 1e-5f));
}

std::uint32_t hash32(std::uint32_t value) {
	value ^= value >> 16;
	value *= 0x7feb352du;
	value ^= value >> 15;
	value *= 0x846ca68bu;
	value ^= value >> 16;
	return value;
}

} // namespace

ReferenceSpringEngine::ReferenceSpringEngine() {
	updateCoefficients();
	resetMotion();
}

void ReferenceSpringEngine::setSampleRate(float newSampleRate) {
	if (!std::isfinite(newSampleRate) || newSampleRate < 1000.f) {
		return;
	}
	sampleRate = newSampleRate;
	sampleTime = 1.f / sampleRate;
	updateCoefficients();
}

void ReferenceSpringEngine::setBreakIn(float amount) {
	if (!std::isfinite(amount)) {
		return;
	}
	const float clamped = clampf(amount, 0.f, 1.f);
	if (clamped == breakIn) {
		return;
	}
	breakIn = clamped;
	updateWearAndSpecimen();
}

void ReferenceSpringEngine::setBreakInLocked(bool locked) {
	breakInLocked = locked;
}

void ReferenceSpringEngine::setSpecimenSeed(std::uint32_t seed) {
	const std::uint32_t validSeed = seed ? seed : 1u;
	if (validSeed == specimenSeed) {
		return;
	}
	specimenSeed = validSeed;
	noiseState = hash32(specimenSeed ^ 0xa341316cu);
	if (!noiseState) {
		noiseState = 1u;
	}
	updateWearAndSpecimen();
}

float ReferenceSpringEngine::specimenUnit(std::uint32_t propertyTag) const {
	const std::uint32_t bits = hash32(specimenSeed ^ propertyTag);
	const float unit = float(bits & 0x00ffffffu) / float(0x00ffffffu);
	return 2.f * unit - 1.f;
}

void ReferenceSpringEngine::updateCoefficients() {
	// The audible lobes are radiation windows over continuously stored modal
	// energy. Fast but rounded edges preserve articulation without manufacturing
	// a new broadband attack at every center crossing.
	radiationAttack = safeDecay(0.0016f, sampleTime);
	radiationRelease = safeDecay(0.0065f, sampleTime);
	energySmoothing = safeDecay(0.004f, sampleTime);
	impactSoftDecay = safeDecay(0.003f, sampleTime);
	impactHardDecay = safeDecay(0.012f, sampleTime);
	impactBrightnessDecay = safeDecay(0.030f, sampleTime);
	strikeLightDecay = safeDecay(0.075f, sampleTime);
	const float maxCutoff = 0.45f * sampleRate;
	impactSoftAlpha = 1.f - std::exp(-2.f * PI * std::min(2400.f, maxCutoff) * sampleTime);
	impactHardAlpha = 1.f - std::exp(-2.f * PI * std::min(6200.f, maxCutoff) * sampleTime);
	impactRejectAlpha = 1.f - std::exp(-2.f * PI * 180.f * sampleTime);
	// Preserve the macro bend in the audio path. The blocker remains only as a
	// slow safety stage for numerical DC rather than a bass-shaping filter.
	dcPole = std::exp(-2.f * PI * 5.f * sampleTime);
	updateWearAndSpecimen();
}

void ReferenceSpringEngine::updateWearAndSpecimen() {
	const float wear = breakIn * breakIn * (3.f - 2.f * breakIn);
	// The physical bend is the conductor of the audible boing. Its 21 Hz resting
	// cycle hardens upward on a strong strike and exposes the modal body at both
	// center crossings, approaching the reference's 45--47 Hz early lobe train.
	baseFrequencyHz = 21.f * lerpf(1.f, 0.88f, wear);
	// Preserve the macroscopic swing long enough for the audible body to form a
	// sequence of distinct boings instead of collapsing into one short impact.
	dampingRatio = 0.010f * lerpf(1.f, 0.72f, wear);
	nonlinearStiffness = 0.32f * lerpf(1.f, 0.78f, wear);
	maximumDisplacement = 2.f * lerpf(1.f, 1.15f, wear);
	baseOmega = 2.f * PI * baseFrequencyHz;
	baseOmegaSq = baseOmega * baseOmega;
	springDamping = 2.f * dampingRatio * baseOmega;
	maximumVelocity = 4.f * baseOmega;
	energyCeiling = std::max(springPotential(maximumDisplacement), 1e-4f);
	// A nonzero floor is essential: the recording's metallic ridges remain
	// audible between lobes. Curvature varies by specimen without an expensive
	// per-sample pow().
	radiationCurvature = clampf(
		0.55f + 0.10f * specimenUnit(0x800u), 0.4f, 0.7f);
	radiationFloor = clampf(
		0.24f + 0.025f * specimenUnit(0x801u), 0.20f, 0.28f);
	radiationAsymmetry = 0.07f * specimenUnit(0x802u);

	const float mountFrequency = 82.f * (1.f + 0.06f * specimenUnit(0x100u));
	const float mountOmega = 2.f * PI * mountFrequency;
	mountOmegaSq = mountOmega * mountOmega;
	mountGamma = LN_1000 / 0.11f;

	for (int i = 0; i < REFERENCE_MODE_COUNT; ++i) {
		const std::uint32_t tag = std::uint32_t(i);
		const float wearFrequency = lerpf(1.f, 0.94f - 0.004f * float(i), wear);
		restingFrequencyHz[i] = BASE_FREQUENCIES[i]
			* (1.f + 0.03f * specimenUnit(0x200u + tag))
			* wearFrequency;
		decayT60Seconds[i] = BASE_DECAYS[i]
			* (1.f + 0.12f * specimenUnit(0x300u + tag))
			* lerpf(1.f, 1.18f, wear);
		impactExcitation[i] = BASE_IMPACT[i]
			* (1.f + 0.08f * specimenUnit(0x400u + tag));
		crossingExcitation[i] = BASE_CROSSING[i]
			* (1.f + 0.10f * specimenUnit(0x500u + tag));
		outputGain[i] = BASE_OUTPUT[i]
			* (1.f + 0.10f * specimenUnit(0x600u + tag));
		directionTilt[i] = specimenUnit(0x700u + tag)
			* lerpf(0.03f, 0.10f, float(i) / float(REFERENCE_MODE_COUNT - 1));
		frequencyWarpDepth[i] = BASE_WARP_DEPTH[i]
			* (1.f + 0.15f * specimenUnit(0x900u + tag));
		modeVelocityLimit[i] = std::max(impactExcitation[i] * 3.f, 300.f);
	}
}

float ReferenceSpringEngine::springPotential(float x) const {
	const float x2 = x * x;
	return 0.5f * baseOmegaSq * x2
		+ 0.25f * nonlinearStiffness * baseOmegaSq * x2 * x2;
}

float ReferenceSpringEngine::normalizedFlexEnergy() const {
	const float energy = 0.5f * springVelocity * springVelocity
		+ springPotential(displacement);
	return energy / std::max(energyCeiling, 1e-6f);
}

float ReferenceSpringEngine::normalizedModalEnergy() const {
	float total = 0.f;
	for (int i = 0; i < REFERENCE_MODE_COUNT; ++i) {
		const float omega = 2.f * PI * restingFrequencyHz[i];
		const float scaledPosition = omega * modes[i].position;
		const float limit = std::max(modeVelocityLimit[i], 1.f);
		total += (modes[i].velocity * modes[i].velocity
			+ scaledPosition * scaledPosition) / (limit * limit);
	}
	return total / float(REFERENCE_MODE_COUNT);
}

void ReferenceSpringEngine::clearDynamicState() {
	displacement = 0.f;
	springVelocity = 0.f;
	acceleration = 0.f;
	previousDisplacement = 0.f;
	armedSide = ArmedSide::None;
	crossingRefractorySamples = 0;
	strikeRefractorySamples = 0;
	lastDirection = 1.f;
	modes = {};
	radiationEnvelope = 0.f;
	smoothedEnergy = 0.f;
	impactEnvelope = 0.f;
	impactBrightness = 0.f;
	impactLowpass = 0.f;
	impactLowReject = 0.f;
	mountPosition = 0.f;
	mountVelocity = 0.f;
	strikeLightEnvelope = 0.f;
	dcPreviousInput = 0.f;
	dcPreviousOutput = 0.f;
	quietTime = 0.f;
}

void ReferenceSpringEngine::resetMotion() {
	clearDynamicState();
	diagnostics = {};
	noiseState = hash32(specimenSeed ^ 0xa341316cu);
	if (!noiseState) {
		noiseState = 1u;
	}
	sleeping = true;
}

void ReferenceSpringEngine::restoreFactoryFresh() {
	breakIn = 0.f;
	updateWearAndSpecimen();
	resetMotion();
}

void ReferenceSpringEngine::reset() {
	breakInLocked = false;
	restoreFactoryFresh();
}

std::uint32_t ReferenceSpringEngine::nextNoiseRandom() {
	std::uint32_t x = noiseState;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	noiseState = x ? x : 1u;
	return noiseState;
}

void ReferenceSpringEngine::strike(float normalizedVelocity) {
	if (!std::isfinite(normalizedVelocity)) {
		return;
	}
	const float clamped = clampf(normalizedVelocity, -1.f, 1.f);
	const float shaped = Engine::shapeMagnitude(std::fabs(clamped));
	if (!(shaped > 0.f)) {
		return;
	}
	if (!breakInLocked) {
		setBreakIn(breakIn + std::pow(shaped, 1.8f) / 1000.f);
	}

	const float direction = std::copysign(1.f, clamped);
	lastDirection = direction;
	const float candidateVelocity = springVelocity + direction * shaped * 150.f;
	springVelocity = clampf(candidateVelocity, -maximumVelocity, maximumVelocity);
	strikeRefractorySamples = std::max(1, int(0.004f * sampleRate));

	const float brightness = shaped * shaped;
	const float modalEnergy = normalizedModalEnergy();
	const float available = clampf(1.f - modalEnergy, 0.f, 1.f);
	for (int i = 0; i < REFERENCE_MODE_COUNT; ++i) {
		const float modeBrightness = lerpf(shaped, brightness,
			float(i) / float(REFERENCE_MODE_COUNT - 1));
		modes[i].velocity += direction * impactExcitation[i]
			* modeBrightness * available;
		modes[i].velocity = clampf(
			modes[i].velocity, -modeVelocityLimit[i], modeVelocityLimit[i]);
	}

	impactEnvelope = std::min(impactEnvelope + shaped, 2.f);
	impactBrightness = std::max(impactBrightness, shaped);
	mountVelocity = clampf(mountVelocity + direction * shaped * 42.f, -160.f, 160.f);
	strikeLightEnvelope = std::max(strikeLightEnvelope, shaped);
	quietTime = 0.f;
	sleeping = false;
}

bool ReferenceSpringEngine::processFlexAndCrossing() {
	previousDisplacement = displacement;
	const float x2 = displacement * displacement;
	const float restoring = baseOmegaSq * displacement
		+ nonlinearStiffness * baseOmegaSq * displacement * x2;
	acceleration = -restoring - springDamping * springVelocity;
	springVelocity += acceleration * sampleTime;
	displacement += springVelocity * sampleTime;

	if (displacement > maximumDisplacement) {
		displacement = maximumDisplacement;
		if (springVelocity > 0.f) springVelocity = 0.f;
	}
	else if (displacement < -maximumDisplacement) {
		displacement = -maximumDisplacement;
		if (springVelocity < 0.f) springVelocity = 0.f;
	}
	springVelocity = clampf(springVelocity, -maximumVelocity, maximumVelocity);

	if (crossingRefractorySamples > 0) --crossingRefractorySamples;
	if (strikeRefractorySamples > 0) --strikeRefractorySamples;

	const float armThreshold = 0.06f * maximumDisplacement;
	if (armedSide == ArmedSide::None) {
		if (displacement >= armThreshold) armedSide = ArmedSide::Positive;
		else if (displacement <= -armThreshold) armedSide = ArmedSide::Negative;
	}

	const float normalizedSpeed = std::fabs(springVelocity)
		/ std::max(maximumVelocity, 1.f);
	const bool fromPositive = armedSide == ArmedSide::Positive
		&& previousDisplacement > 0.f && displacement <= 0.f;
	const bool fromNegative = armedSide == ArmedSide::Negative
		&& previousDisplacement < 0.f && displacement >= 0.f;
	const bool geometricCrossing = fromPositive || fromNegative;
	const bool crossed = geometricCrossing
		&& normalizedSpeed > 0.025f
		&& crossingRefractorySamples == 0
		&& strikeRefractorySamples == 0;
	if (geometricCrossing) {
		armedSide = ArmedSide::None;
	}
	if (crossed) {
		lastDirection = springVelocity >= 0.f ? 1.f : -1.f;
		crossingRefractorySamples = std::max(1, int(0.004f * sampleRate));
		++diagnostics.crossingCount;
		exciteCrossing(normalizedSpeed);
	}
	return crossed;
}

void ReferenceSpringEngine::exciteCrossing(float normalizedSpeed) {
	const float available = clampf(1.f - normalizedModalEnergy(), 0.f, 1.f);
	// Crossing excitation is reinforcement, not the source of each lobe. The
	// modal body is filled by the strike and persists on its own; this restrained
	// transfer merely keeps the slow and fast structures perceptually coupled.
	const float strength = 0.22f * normalizedSpeed * available;
	for (int i = 0; i < REFERENCE_MODE_COUNT; ++i) {
		const float directional = 1.f + lastDirection * directionTilt[i];
		modes[i].velocity += crossingExcitation[i] * strength * directional;
		modes[i].velocity = clampf(
			modes[i].velocity, -modeVelocityLimit[i], modeVelocityLimit[i]);
	}
}

float ReferenceSpringEngine::processModes() {
	const float energy = clampf(smoothedEnergy, 0.f, 1.f);
	float output = 0.f;
	for (int i = 0; i < REFERENCE_MODE_COUNT; ++i) {
		// Measured ridges settle by different amounts. A small mode-specific
		// warp avoids the synthetic global pitch dive of the previous model.
		const float pitchWarp = 1.f + frequencyWarpDepth[i] * energy;
		const float frequency = std::min(
			restingFrequencyHz[i] * pitchWarp,
			0.18f * sampleRate);
		const float omega = 2.f * PI * frequency;
		const float gamma = LN_1000 / std::max(decayT60Seconds[i], 1e-4f);
		Mode& mode = modes[i];
		const float modeAcceleration = -omega * omega * mode.position
			- 2.f * gamma * mode.velocity;
		mode.velocity += modeAcceleration * sampleTime;
		mode.position += mode.velocity * sampleTime;
		mode.velocity = clampf(
			mode.velocity, -modeVelocityLimit[i], modeVelocityLimit[i]);
		const float directional = 1.f + lastDirection * directionTilt[i];
		output += mode.position * outputGain[i] * directional;
	}
	return output;
}

float ReferenceSpringEngine::processImpact() {
	const float noise = float(nextNoiseRandom() & 0x00ffffffu)
		/ float(0x00800000u) - 1.f;
	const float brightness = clampf(impactBrightness, 0.f, 1.f);
	const float alpha = lerpf(impactSoftAlpha, impactHardAlpha, brightness);
	impactLowpass += (noise - impactLowpass) * alpha;
	impactLowReject += (impactLowpass - impactLowReject) * impactRejectAlpha;
	const float output = (impactLowpass - impactLowReject)
		* impactEnvelope * (0.16f + 0.24f * brightness);
	impactEnvelope *= lerpf(impactSoftDecay, impactHardDecay, brightness);
	impactBrightness *= impactBrightnessDecay;
	return output;
}

float ReferenceSpringEngine::processMount() {
	const float mountAcceleration = -mountOmegaSq * mountPosition
		- 2.f * mountGamma * mountVelocity;
	mountVelocity += mountAcceleration * sampleTime;
	mountPosition += mountVelocity * sampleTime;
	return mountPosition * 0.36f;
}

float ReferenceSpringEngine::processFlexAudio() {
	const float normalizedVelocity = springVelocity / std::max(maximumVelocity, 1.f);
	const float normalizedAcceleration = clampf(acceleration / 24000.f, -1.f, 1.f);
	// Velocity supplies the weight of the whole spring moving while acceleration
	// adds the harder edge at each turn. Do not high-pass this signal: the
	// physical bend is an intentional audible part of the model.
	return 0.12f * normalizedVelocity + 0.055f * normalizedAcceleration;
}

float ReferenceSpringEngine::processDcBlocker(float input) {
	const float output = input - dcPreviousInput + dcPole * dcPreviousOutput;
	dcPreviousInput = input;
	dcPreviousOutput = output;
	return output;
}

bool ReferenceSpringEngine::allFinite() const {
	if (!std::isfinite(displacement) || !std::isfinite(springVelocity)
		|| !std::isfinite(acceleration) || !std::isfinite(radiationEnvelope)
		|| !std::isfinite(smoothedEnergy) || !std::isfinite(impactEnvelope)
		|| !std::isfinite(impactBrightness) || !std::isfinite(impactLowpass)
		|| !std::isfinite(impactLowReject) || !std::isfinite(mountPosition)
		|| !std::isfinite(mountVelocity) || !std::isfinite(strikeLightEnvelope)
		|| !std::isfinite(dcPreviousInput) || !std::isfinite(dcPreviousOutput)) {
		return false;
	}
	for (const Mode& mode : modes) {
		if (!std::isfinite(mode.position) || !std::isfinite(mode.velocity)) {
			return false;
		}
	}
	return true;
}

bool ReferenceSpringEngine::belowSleepThreshold(float outputVolts) const {
	if (normalizedFlexEnergy() >= 1e-8f || normalizedModalEnergy() >= 1e-8f) {
		return false;
	}
	return impactEnvelope < 1e-5f
		&& std::fabs(mountPosition) < 1e-5f
		&& std::fabs(mountVelocity) < 1e-5f
		&& strikeLightEnvelope < 1e-5f
		&& std::fabs(outputVolts) < 1e-4f;
}

void ReferenceSpringEngine::recoverFromNonFinite() {
	resetMotion();
}

Frame ReferenceSpringEngine::process(float requestedSampleTime) {
	(void) requestedSampleTime;
	if (sleeping) {
		return {};
	}

	processFlexAndCrossing();
	const float flexEnergy = clampf(normalizedFlexEnergy(), 0.f, 1.f);
	smoothedEnergy = energySmoothing * smoothedEnergy
		+ (1.f - energySmoothing) * flexEnergy;
	// Radiation follows bend phase more than absolute swing magnitude. Using a
	// phase-normalized velocity keeps later windows distinct instead of making
	// their peaks collapse with the macroscopic oscillator's energy.
	const float phaseVelocity = std::fabs(springVelocity);
	const float phaseDisplacement =
		baseOmega * std::fabs(displacement);
	const float articulationVelocity = phaseVelocity
		/ std::max(phaseVelocity + phaseDisplacement, 1e-6f);
	const float phasePulse = articulationVelocity
		* lerpf(1.f, articulationVelocity, radiationCurvature);
	const float targetPulse = phasePulse * (0.72f + 0.28f * flexEnergy);
	const float smoothing = targetPulse > radiationEnvelope
		? radiationAttack : radiationRelease;
	radiationEnvelope = smoothing * radiationEnvelope
		+ (1.f - smoothing) * targetPulse;
	const float directionalRadiation = 1.f + lastDirection * radiationAsymmetry;
	const float radiationGate = (radiationFloor
		+ (1.f - radiationFloor) * radiationEnvelope)
		* directionalRadiation;

	const float modal = processModes();
	const float impact = processImpact();
	const float mount = processMount();
	const float flex = processFlexAudio();
	float signal = impact * 0.45f + mount * 0.16f + flex * 0.50f
		+ modal * radiationGate * 1.90f;
	signal = processDcBlocker(signal);
	// Keep the safety saturation from turning the strengthened lower modes
	// into a synthetic spray of upper harmonics.
	const float outputVolts = 5.f * levi_math::tanhAudio(signal * 2.2f);
	strikeLightEnvelope *= strikeLightDecay;

	diagnostics.radiationGate = radiationGate;
	diagnostics.normalizedModalEnergy = normalizedModalEnergy();

	if (!allFinite() || !std::isfinite(outputVolts)) {
		recoverFromNonFinite();
		return {};
	}

	Frame frame;
	frame.outputVolts = outputVolts;
	frame.displacement = displacement;
	frame.velocity = springVelocity / std::max(maximumVelocity, 1.f);
	frame.energy = flexEnergy;
	frame.strikeLight = clampf(strikeLightEnvelope, 0.f, 1.f);
	frame.sleeping = false;

	if (belowSleepThreshold(outputVolts)) quietTime += sampleTime;
	else quietTime = 0.f;
	if (quietTime >= 0.050f) {
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
