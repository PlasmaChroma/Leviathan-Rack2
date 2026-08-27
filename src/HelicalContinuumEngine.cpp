#include "HelicalContinuumEngine.hpp"

#include <algorithm>
#include <cmath>

namespace doorstop {
namespace {

constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_FREQUENCIES {{
	22.f, 190.f, 315.f, 520.f, 860.f, 1450.f, 2500.f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_SPLITS {{
	0.012f, 0.0060f, 0.0055f, 0.0045f, 0.0038f, 0.0030f, 0.0025f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_T60 {{
	3.4f, 6.2f, 5.8f, 5.0f, 4.0f, 2.8f, 1.7f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_TIP {{
	1.f, 0.42f, -0.34f, 0.29f, -0.22f, 0.16f, -0.11f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_RADIATION {{
	0.0025f, 0.30f, 0.38f, 0.47f, 0.42f, 0.31f, 0.20f
}};

float clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

} // namespace

void HelicalContinuumEngine::reset() {
	breakIn = 0.f;
	breakInLocked = false;
	updateSpecimenCoefficients();
	clearDynamicState();
}

void HelicalContinuumEngine::resetMotion() {
	clearDynamicState();
}

void HelicalContinuumEngine::restoreFactoryFresh() {
	breakIn = 0.f;
	updateSpecimenCoefficients();
	clearDynamicState();
}

void HelicalContinuumEngine::setSampleRate(float newSampleRate) {
	if (!std::isfinite(newSampleRate) || newSampleRate < 1000.f) return;
	sampleRate = newSampleRate;
	sampleTime = 1.f / sampleRate;
	updateCoefficients();
}

void HelicalContinuumEngine::setBreakIn(float amount) {
	if (!std::isfinite(amount)) return;
	breakIn = clamp01(amount);
	updateSpecimenCoefficients();
}

void HelicalContinuumEngine::setBreakInLocked(bool locked) {
	breakInLocked = locked;
}

void HelicalContinuumEngine::setSpecimenSeed(std::uint32_t seed) {
	specimenSeed = seed ? seed : 1u;
	updateSpecimenCoefficients();
}

float HelicalContinuumEngine::specimenUnit(std::uint32_t tag) const {
	std::uint32_t x = specimenSeed ^ (tag * 0x9e3779b9u);
	x ^= x >> 16;
	x *= 0x7feb352du;
	x ^= x >> 15;
	x *= 0x846ca68bu;
	x ^= x >> 16;
	return float(x >> 8) * (1.f / 16777215.f);
}

void HelicalContinuumEngine::updateSpecimenCoefficients() {
	const float stiffnessTrait = 0.96f + 0.08f * specimenUnit(1u);
	const float dampingTrait = 0.88f + 0.24f * specimenUnit(2u);
	const float wearStiffness = 1.f - 0.025f * breakIn;
	const float wearDamping = 1.f - 0.12f * breakIn;
	for (int pair = 0; pair < PAIR_COUNT; ++pair) {
		const float pairVariation = 0.985f + 0.030f * specimenUnit(20u + pair);
		const float splitVariation = 0.75f + 0.50f * specimenUnit(40u + pair);
		const float center = PAIR_FREQUENCIES[pair] * stiffnessTrait
			* wearStiffness * pairVariation;
		const float split = PAIR_SPLITS[pair] * splitVariation;
		const float t60 = PAIR_T60[pair] * dampingTrait * wearDamping;
		for (int axis = 0; axis < 2; ++axis) {
			const int index = 2 * pair + axis;
			const float frequency = center * (1.f + (axis ? split : -split));
			const float omega = 2.f * PI * frequency;
			omegaSq[index] = omega * omega;
			damping[index] = 2.f * 6.907755278982137f / std::max(t60, 0.1f);
			const float orientation = axis == 0 ? 1.f : (0.72f + 0.18f * specimenUnit(60u + pair));
			tipParticipation[index] = PAIR_TIP[pair] * orientation;
			const float sign = ((pair + axis) & 1) ? -1.f : 1.f;
			radiationWeight[index] = PAIR_RADIATION[pair] * sign
				* (0.82f + 0.30f * specimenUnit(80u + index));
			strainWeight[index] = pair == 0 ? 1.f : 0.08f / float(pair + 1);
		}
	}
	updateCoefficients();
}

void HelicalContinuumEngine::updateCoefficients() {
	dcPole = std::exp(-2.f * PI * 8.f / sampleRate);
	strikeLightDecay = std::exp(-1.f / (0.075f * sampleRate));
}

void HelicalContinuumEngine::clearDynamicState() {
	for (ModalState& mode : modes) mode = {};
	capPosition = {};
	capVelocity = {};
	strikePulse = {};
	stiffnessScale = 1.f;
	controlCounter = 0;
	dcPreviousInput = 0.f;
	dcPreviousOutput = 0.f;
	strikeLight = 0.f;
	quietTime = 0.f;
	sleeping = true;
}

void HelicalContinuumEngine::strike(float normalizedVelocity) {
	if (!std::isfinite(normalizedVelocity)) return;
	const float velocity = std::max(-1.f, std::min(normalizedVelocity, 1.f));
	const float magnitude = std::fabs(velocity);
	if (magnitude <= 0.f) return;

	const float durationSeconds = 0.0022f - 0.0015f * magnitude;
	const int substeps = std::max(4, int(durationSeconds * sampleRate * SUBSTEPS + 0.5f));
	const float phaseStep = 2.f * PI / float(substeps);
	const float misalignment = -0.20f + 0.40f * specimenUnit(101u);
	const float inverseLength = 1.f / std::sqrt(1.f + misalignment * misalignment);
	strikePulse.magnitude += 1450.f * magnitude * (0.40f + 0.60f * magnitude);
	strikePulse.magnitude = std::min(strikePulse.magnitude, 2200.f);
	strikePulse.directionX = (velocity < 0.f ? -1.f : 1.f) * inverseLength;
	strikePulse.directionY = misalignment * inverseLength;
	strikePulse.cosine = 1.f;
	strikePulse.sine = 0.f;
	strikePulse.cosineStep = std::cos(phaseStep);
	strikePulse.sineStep = std::sin(phaseStep);
	strikePulse.remainingSubsteps = substeps;
	strikeLight = std::max(strikeLight, magnitude);
	sleeping = false;
	quietTime = 0.f;
	if (!breakInLocked) {
		breakIn = std::min(1.f, breakIn + 0.00008f * magnitude);
	}
}

float HelicalContinuumEngine::calculateEnergy() const {
	double energy = 0.0;
	for (int i = 0; i < MODE_COUNT; ++i) {
		energy += 0.5 * double(modes[i].velocity) * modes[i].velocity
			+ 0.5 * double(omegaSq[i]) * modes[i].position * modes[i].position;
	}
	energy += 0.5 * double(capVelocity.x) * capVelocity.x
		+ 0.5 * double(capVelocity.y) * capVelocity.y;
	return float(std::min(energy, 1.0e12));
}

float HelicalContinuumEngine::processSubstep(float h) {
	float externalX = 0.f;
	float externalY = 0.f;
	if (strikePulse.remainingSubsteps > 0) {
		const float envelope = 0.5f * (1.f - strikePulse.cosine);
		externalX = strikePulse.magnitude * envelope * strikePulse.directionX;
		externalY = strikePulse.magnitude * envelope * strikePulse.directionY;
		const float nextCosine = strikePulse.cosine * strikePulse.cosineStep
			- strikePulse.sine * strikePulse.sineStep;
		strikePulse.sine = strikePulse.sine * strikePulse.cosineStep
			+ strikePulse.cosine * strikePulse.sineStep;
		strikePulse.cosine = nextCosine;
		--strikePulse.remainingSubsteps;
	}

	Vec2 tipPosition {};
	Vec2 tipVelocity {};
	for (int pair = 0; pair < PAIR_COUNT; ++pair) {
		const int x = 2 * pair;
		const int y = x + 1;
		tipPosition.x += tipParticipation[x] * modes[x].position;
		tipPosition.y += tipParticipation[y] * modes[y].position;
		tipVelocity.x += tipParticipation[x] * modes[x].velocity;
		tipVelocity.y += tipParticipation[y] * modes[y].velocity;
	}

	const float capOmega = 2.f * PI * 105.f;
	const float capStiffness = capOmega * capOmega;
	const float capDamping = 2.f * 0.34f * capOmega;
	const float attachmentX = capStiffness * (capPosition.x - tipPosition.x)
		+ capDamping * (capVelocity.x - tipVelocity.x);
	const float attachmentY = capStiffness * (capPosition.y - tipPosition.y)
		+ capDamping * (capVelocity.y - tipVelocity.y);
	capVelocity.x += h * (externalX - attachmentX);
	capVelocity.y += h * (externalY - attachmentY);
	capPosition.x += h * capVelocity.x;
	capPosition.y += h * capVelocity.y;

	float radiation = 0.f;
	for (int pair = 0; pair < PAIR_COUNT; ++pair) {
		for (int axis = 0; axis < 2; ++axis) {
			const int index = 2 * pair + axis;
			const float attachment = axis == 0 ? attachmentX : attachmentY;
			const float generalizedForce = tipParticipation[index] * attachment;
			ModalState& mode = modes[index];
			mode.acceleration = generalizedForce - damping[index] * mode.velocity
				- stiffnessScale * omegaSq[index] * mode.position;
			mode.velocity += h * mode.acceleration;
			mode.position += h * mode.velocity;
			radiation += radiationWeight[index] * mode.acceleration;
		}
	}
	return radiation;
}

float HelicalContinuumEngine::processDcBlocker(float input) {
	const float output = input - dcPreviousInput + dcPole * dcPreviousOutput;
	dcPreviousInput = input;
	dcPreviousOutput = output;
	return output;
}

bool HelicalContinuumEngine::allFinite() const {
	if (!std::isfinite(capPosition.x) || !std::isfinite(capPosition.y)
		|| !std::isfinite(capVelocity.x) || !std::isfinite(capVelocity.y)) return false;
	for (const ModalState& mode : modes) {
		if (!std::isfinite(mode.position) || !std::isfinite(mode.velocity)
			|| !std::isfinite(mode.acceleration)) return false;
	}
	return std::isfinite(dcPreviousOutput);
}

void HelicalContinuumEngine::recoverFromNonFinite() {
	clearDynamicState();
}

Frame HelicalContinuumEngine::process(float requestedSampleTime) {
	(void) requestedSampleTime;
	if (sleeping && strikePulse.remainingSubsteps == 0) return {};

	if (++controlCounter >= 16) {
		controlCounter = 0;
		double strain = 0.0;
		for (int i = 0; i < MODE_COUNT; ++i) {
			strain += double(strainWeight[i]) * omegaSq[i]
				* modes[i].position * modes[i].position;
		}
		stiffnessScale = 1.f + 0.055f * float(strain / (1.0 + strain));
	}

	const float h = sampleTime / float(SUBSTEPS);
	float radiation = 0.f;
	for (int substep = 0; substep < SUBSTEPS; ++substep) {
		radiation += processSubstep(h);
	}
	radiation *= 0.5f;
	if (!allFinite()) {
		recoverFromNonFinite();
		return {};
	}

	// Calibrated linear gain. Ordinary single strikes remain comfortably below
	// the safety knee so listening reflects the mechanics, not saturation.
	const float preconditioned = processDcBlocker(radiation * 0.0068f);
	const float output = 5.f * std::tanh(preconditioned * 0.55f);
	const float energy = calculateEnergy();
	const float normalizedEnergy = std::min(1.f, energy * 0.000015f);
	strikeLight *= strikeLightDecay;
	const bool quiet = strikePulse.remainingSubsteps == 0
		&& energy < 2.0e-7f && std::fabs(output) < 1.0e-4f;
	quietTime = quiet ? quietTime + sampleTime : 0.f;
	const bool wasSleeping = sleeping;
	if (quietTime >= 0.075f) {
		clearDynamicState();
		sleeping = true;
	}

	Frame frame;
	frame.outputVolts = output;
	frame.displacement = std::max(-2.f, std::min(modes[0].position * 1.25f, 2.f));
	frame.velocity = std::max(-1.f, std::min(modes[0].velocity * 0.025f, 1.f));
	frame.energy = normalizedEnergy;
	frame.strikeLight = strikeLight;
	frame.sleeping = sleeping;
	frame.enteredSleep = !wasSleeping && sleeping;
	return frame;
}

} // namespace doorstop
