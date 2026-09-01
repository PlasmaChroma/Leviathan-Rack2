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
constexpr float RADIATION_VELOCITY_COEFFICIENT = 2.f * 3.14159265358979323846f * 140.f;

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

void HelicalContinuumEngine::setObserverVariant(
	HelicalObserverVariant variant) {
	observerVariant = variant < HelicalObserverVariant::Count
		? variant : HelicalObserverVariant::Fixed;
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
			omega[index] = 2.f * PI * frequency;
			omegaSq[index] = omega[index] * omega[index];
			damping[index] = 2.f * 6.907755278982137f / std::max(t60, 0.1f);
			const float orientation = axis == 0 ? 1.f : (0.72f + 0.18f * specimenUnit(60u + pair));
			tipParticipation[index] = PAIR_TIP[pair] * orientation;
			const float sign = ((pair + axis) & 1) ? -1.f : 1.f;
			radiationWeight[index] = PAIR_RADIATION[pair] * sign
				* (0.82f + 0.30f * specimenUnit(80u + index));
			const float observerX = 2.f * specimenUnit(120u + index) - 1.f;
			const float observerY = 2.f * specimenUnit(140u + index) - 1.f;
			const float observerNorm = std::max(
				0.001f, std::fabs(observerX) + std::fabs(observerY));
			observerDirectionX[index] = observerX / observerNorm;
			observerDirectionY[index] = observerY / observerNorm;
			strainWeight[index] = pair == 0 ? 1.f : 0.08f / float(pair + 1);
		}
	}
	updateCoefficients();
}

void HelicalContinuumEngine::updateCoefficients() {
	dcPole = std::exp(-2.f * PI * 8.f / sampleRate);
	strikeLightDecay = std::exp(-1.f / (0.075f * sampleRate));
	visualEnvelopeDecay = std::exp(-1.f / (0.18f * sampleRate));
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
	visualAudibleEnvelope = 0.f;
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
	const bool paired = observerVariant != HelicalObserverVariant::NoPairs;
	const float misalignment = paired
		? -0.20f + 0.40f * specimenUnit(101u) : 0.f;
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
	const bool paired = observerVariant != HelicalObserverVariant::NoPairs;
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
		if (paired) tipPosition.y += tipParticipation[y] * modes[y].position;
		tipVelocity.x += tipParticipation[x] * modes[x].velocity;
		if (paired) tipVelocity.y += tipParticipation[y] * modes[y].velocity;
	}
	if (!paired) {
		capPosition.y = 0.f;
		capVelocity.y = 0.f;
		externalY = 0.f;
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

	float observerX = 0.f;
	float observerY = 0.f;
	if (observerVariant == HelicalObserverVariant::Crossing
		|| observerVariant == HelicalObserverVariant::Mixed) {
		observerX += modes[0].velocity;
		observerY += modes[1].velocity;
	}
	if (observerVariant == HelicalObserverVariant::Bend
		|| observerVariant == HelicalObserverVariant::Mixed) {
		observerX += omega[0] * modes[0].position;
		observerY += omega[1] * modes[1].position;
	}
	const float observerNorm = std::fabs(observerX) + std::fabs(observerY);
	if (observerNorm > 1.0e-7f) {
		const float inverseObserverNorm = 1.f / observerNorm;
		observerX *= inverseObserverNorm;
		observerY *= inverseObserverNorm;
	}

	float radiation = 0.f;
	for (int pair = 0; pair < PAIR_COUNT; ++pair) {
		for (int axis = 0; axis < 2; ++axis) {
			const int index = 2 * pair + axis;
			if (!paired && axis == 1) {
				modes[index] = {};
				continue;
			}
			const float attachment = axis == 0 ? attachmentX : attachmentY;
			const float generalizedForce = tipParticipation[index] * attachment;
			ModalState& mode = modes[index];
			mode.acceleration = generalizedForce - damping[index] * mode.velocity
				- stiffnessScale * omegaSq[index] * mode.position;
			mode.velocity += h * mode.acceleration;
			mode.position += h * mode.velocity;
			// A fixed observer hears both local acceleration and structural
			// velocity. The velocity term keeps the lower body modes audible
			// through their mechanical decay instead of reducing V3 to a short,
			// acceleration-dominated transient.
			const float observedMotion = mode.acceleration
				+ RADIATION_VELOCITY_COEFFICIENT * mode.velocity;
			float observerGain = 1.f;
			if (observerVariant == HelicalObserverVariant::Crossing
				|| observerVariant == HelicalObserverVariant::Bend
				|| observerVariant == HelicalObserverVariant::Mixed) {
				const float movingProjection =
					observerDirectionX[index] * observerX
					+ observerDirectionY[index] * observerY;
				observerGain = 0.28f + 1.08f * movingProjection;
			}
			radiation += radiationWeight[index] * observerGain * observedMotion;
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
	const float audibleVisualTarget = std::min(1.f, std::fabs(output) * 0.8f);
	visualAudibleEnvelope = std::max(
		audibleVisualTarget, visualAudibleEnvelope * visualEnvelopeDecay);
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
	// Modal coordinates are expressed in mechanical-model units, several
	// orders of magnitude below the display's +/-2 visual range. Project the
	// retained fundamental into visual units without altering the audio state.
	constexpr float VISUAL_DISPLACEMENT_GAIN = 450.f;
	constexpr float VISUAL_VELOCITY_GAIN = 1.625f;
	frame.displacement = std::max(-2.f, std::min(
		modes[0].position * VISUAL_DISPLACEMENT_GAIN * visualAudibleEnvelope, 2.f));
	frame.velocity = std::max(-1.f, std::min(
		modes[0].velocity * VISUAL_VELOCITY_GAIN * visualAudibleEnvelope, 1.f));
	frame.energy = normalizedEnergy;
	frame.strikeLight = strikeLight;
	frame.sleeping = sleeping;
	frame.enteredSleep = !wasSleeping && sleeping;
	return frame;
}

} // namespace doorstop
