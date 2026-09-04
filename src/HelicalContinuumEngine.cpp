#include "HelicalContinuumEngine.hpp"

#include <algorithm>
#include <cmath>

namespace doorstop {
namespace {

// Keep the retained body centered in the doorstop's boing range. The upper
// pairs are radiation detail rather than a second, long-lived bell voice.
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_FREQUENCIES {{
	28.f, 75.f, 175.f, 285.f, 347.f, 545.f, 713.f, 938.f, 1168.f, 1454.f, 2328.f, 3864.f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_SPLITS {{
	0.010f, 0.0055f, 0.0050f, 0.0045f, 0.0040f, 0.0035f, 0.0030f, 0.0028f, 0.0025f, 0.0022f, 0.0018f, 0.0015f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_T60 {{
	3.2f, 0.22f, 3.2f, 4.0f, 5.5f, 7.0f, 6.5f, 5.8f, 3.6f, 2.6f, 1.4f, 0.65f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_OUTPUT {{
	0.002f, 0.03f, 0.04f, 0.06f, 0.12f, 0.50f, 0.55f, 0.48f, 0.85f, 0.65f, 0.25f, 0.12f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> DARK_OUTPUT_SCALE {{
	1.10f, 1.10f, 1.05f, 1.08f, 1.12f, 1.30f, 1.20f, 0.95f, 0.65f, 0.40f, 0.18f, 0.06f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> DARK_T60_SCALE {{
	1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.92f, 0.78f, 0.65f, 0.48f, 0.35f
}};
// Deep Swing is a bending-body voice rather than a harder mallet striking a
// bell. Favor the first few structural pairs and let the upper contact detail
// leave before the visible low bend has settled.
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> DEEP_HARD_OUTPUT_SCALE {{
	1.10f, 1.10f, 1.10f, 1.00f, 1.00f, 0.90f,
	0.85f, 0.75f, 0.65f, 0.55f, 0.45f, 0.35f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> DEEP_HARD_T60_SCALE {{
	1.10f, 1.00f, 1.05f, 0.95f, 0.90f, 0.75f,
	0.70f, 0.60f, 0.55f, 0.50f, 0.45f, 0.40f
}};
constexpr std::array<float, HelicalContinuumEngine::PAIR_COUNT> PAIR_TIP {{
	1.f, 0.34f, -0.31f, 0.29f, -0.27f, 0.24f, -0.21f, 0.18f, -0.15f, 0.12f, -0.09f, 0.06f
}};
constexpr float RADIATION_VELOCITY_COEFFICIENT =
	2.f * 3.14159265358979323846f * 140.f;
// Representative modal-plus-cap energy from a strong V3 strike. This is a
// visual reference ceiling, not a dynamics limiter: harder/retriggered motion
// may exceed it and simply fills the meter.
constexpr float VISUAL_ENERGY_REFERENCE = 0.25f;

float clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

} // namespace

void HelicalContinuumEngine::reset() {
	breakIn = 0.f;
	breakInLocked = false;
	tuningVariant = HelicalTuningVariant::BoingProbe;
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

void HelicalContinuumEngine::setTuningVariant(HelicalTuningVariant variant) {
	if (variant >= HelicalTuningVariant::Count) {
		variant = HelicalTuningVariant::BoingProbe;
	}
	if (variant == tuningVariant) return;
	tuningVariant = variant;
	updateSpecimenCoefficients();
	clearDynamicState();
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
	const bool dark = tuningVariant != HelicalTuningVariant::BoingProbe;
	const float tuningFrequencyScale = dark ? 0.82f : 1.f;
	const float stiffnessTrait = 0.96f + 0.08f * specimenUnit(1u);
	const float dampingTrait = 0.88f + 0.24f * specimenUnit(2u);
	const float wearStiffness = 1.f - 0.025f * breakIn;
	const float wearDamping = 1.f - 0.12f * breakIn;

	gapClearance = 0.0016f * (0.90f + 0.20f * specimenUnit(15u));
	contactStiffness = 1.8e7f * (0.85f + 0.30f * specimenUnit(16u));

	for (int pair = 0; pair < PAIR_COUNT; ++pair) {
		const float pairVariation = 0.985f + 0.030f * specimenUnit(20u + pair);
		const float splitVariation = 0.75f + 0.50f * specimenUnit(40u + pair);
		const float center = PAIR_FREQUENCIES[pair] * tuningFrequencyScale * stiffnessTrait
			* wearStiffness * pairVariation;
		const float split = PAIR_SPLITS[pair] * splitVariation;
		const float t60 = PAIR_T60[pair]
			* (dark ? DARK_T60_SCALE[pair] : 1.f)
			* dampingTrait * wearDamping;

		for (int axis = 0; axis < 2; ++axis) {
			const int index = 2 * pair + axis;
			const float frequency = center * (1.f + (axis ? split : -split));
			omega[index] = 2.f * PI * frequency;
			omegaSq[index] = omega[index] * omega[index];
			damping[index] = 2.f * 6.907755278982137f / std::max(t60, 0.1f);

			const float orientation = axis == 0 ? 1.f : (0.72f + 0.18f * specimenUnit(60u + pair));
			tipParticipation[index] = PAIR_TIP[pair] * orientation;

			const float sign = ((pair + axis) & 1) ? -1.f : 1.f;
			radiationWeight[index] = PAIR_OUTPUT[pair]
				* (dark ? DARK_OUTPUT_SCALE[pair] : 1.f) * sign
				* (0.85f + 0.30f * specimenUnit(80u + index));

			contactParticipation[index] = pair == 0 ? 1.f
				: PAIR_TIP[pair] * (0.75f + 0.35f * specimenUnit(100u + index));

			const float observerX = 2.f * specimenUnit(120u + index) - 1.f;
			const float observerY = 2.f * specimenUnit(140u + index) - 1.f;
			const float observerNorm = std::max(
				0.001f, std::fabs(observerX) + std::fabs(observerY));
			observerDirectionX[index] = observerX / observerNorm;
			observerDirectionY[index] = observerY / observerNorm;

			strainWeight[index] = pair == 0 ? 1.f : 0.06f / float(pair + 1);
		}
	}
	updateCoefficients();
}

void HelicalContinuumEngine::updateCoefficients() {
	dcPole = std::exp(-2.f * PI * 8.f / sampleRate);
	strikeLightDecay = std::exp(-1.f / (0.075f * sampleRate));
	visualEnvelopeDecay = std::exp(-1.f / (0.18f * sampleRate));
	hardStrikeDecay = std::exp(-1.f / (2.2f * sampleRate));
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
	hardStrikeDrive = 0.f;
	quietTime = 0.f;
	previousContactForce = 0.f;
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
	// Pulse duration continues to shorten for a brighter hard impact, but above
	// 80% it previously shortened faster than force grew. Compensate only that
	// top region so total impulse and stored motion do not fold back before max.
	const float maximumRange = clamp01((magnitude - 0.80f) / 0.20f);
	const float maximumBlend = maximumRange * maximumRange
		* (3.f - 2.f * maximumRange);
	const float maximumImpulseBoost = 1.f + 0.35f * maximumBlend;

	strikePulse.magnitude += 1450.f * magnitude
		* (0.40f + 0.60f * magnitude) * maximumImpulseBoost;
	strikePulse.magnitude = std::min(strikePulse.magnitude, 2200.f);
	strikePulse.directionX = (velocity < 0.f ? -1.f : 1.f) * inverseLength;
	strikePulse.directionY = misalignment * inverseLength;
	strikePulse.cosine = 1.f;
	strikePulse.sine = 0.f;
	strikePulse.cosineStep = std::cos(phaseStep);
	strikePulse.sineStep = std::sin(phaseStep);
	strikePulse.remainingSubsteps = substeps;

	strikeLight = std::max(strikeLight, magnitude);
	if (tuningVariant == HelicalTuningVariant::DeepSwing) {
		const float hardAmount = clamp01((magnitude - 0.65f) / 0.35f);
		hardStrikeDrive = std::max(hardStrikeDrive, hardAmount * hardAmount);
	}
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
		tipVelocity.x += tipParticipation[x] * modes[x].velocity;
		tipPosition.y += tipParticipation[y] * modes[y].position;
		tipVelocity.y += tipParticipation[y] * modes[y].velocity;
	}

	const float capOmega = 2.f * PI * 105.f;
	const float capStiffness = capOmega * capOmega;
	// The rubber cap rounds the strike but must not drain the long-lived wire
	// body through the shared attachment coordinate.
	const float capDamping = 2.f * 0.001f * capOmega;
	const float attachmentX = capStiffness * (capPosition.x - tipPosition.x)
		+ capDamping * (capVelocity.x - tipVelocity.x);
	const float attachmentY = capStiffness * (capPosition.y - tipPosition.y)
		+ capDamping * (capVelocity.y - tipVelocity.y);
	capVelocity.x += h * (externalX - attachmentX);
	capVelocity.y += h * (externalY - attachmentY);
	capPosition.x += h * capVelocity.x;
	capPosition.y += h * capVelocity.y;

	// A unilateral coil force opposes excessive bend and therefore feeds back
	// into the low mechanical state. Its change at contact onset is the small
	// broadband drive; sustained force is not treated as a repeating impact.
	const float bend = modes[0].position;
	const float penetration = std::max(0.f, std::fabs(bend) - gapClearance);
	float contactForce = 0.f;
	if (penetration > 0.f) {
		const float sign = bend < 0.f ? -1.f : 1.f;
		const float closingSpeed = sign * modes[0].velocity;
		contactForce = sign * (
			contactStiffness * penetration * penetration
			+ contactDamping * std::max(0.f, closingSpeed) * penetration
		);
	}
	const float contactOnset = contactForce - previousContactForce;
	previousContactForce = contactForce;

	const float r2 = modes[0].position * modes[0].position + modes[1].position * modes[1].position;
	const float bendStrain = 18000.f * r2;
	const float normalizedBend = bendStrain / (1.f + bendStrain);

	const float bendMotion = omega[0] * std::fabs(modes[0].position)
		+ omega[1] * std::fabs(modes[1].position);
	const float crossingMotion = std::fabs(modes[0].velocity)
		+ std::fabs(modes[1].velocity);
	const float motionSum = bendMotion + crossingMotion;
	const float crossingPhase = motionSum > 1.0e-8f
		? crossingMotion / motionSum : 0.f;
	const float crossingLobe = crossingPhase * crossingPhase;
	const float direction = modes[0].velocity < 0.f ? -1.f : 1.f;

	float radiation = 0.f;
	for (int pair = 0; pair < PAIR_COUNT; ++pair) {
		for (int axis = 0; axis < 2; ++axis) {
			const int index = 2 * pair + axis;
			ModalState& mode = modes[index];
			const float attachment = axis == 0 ? attachmentX : attachmentY;
			const float capTransmission = axis == 0 ? externalX : externalY;
			// The compliant cap carries most of the force, while a small prompt
			// component transmitted at the same boundary excites the wire above
			// the cap resonance instead of turning the cap into an unintended
			// brick-wall low-pass filter.
			float generalizedForce = tipParticipation[index]
				* (attachment + 0.08f * capTransmission);
			if (axis == 0) {
				generalizedForce -= contactParticipation[index] * contactForce;
				if (pair > 0) {
					generalizedForce -= 0.30f * contactParticipation[index] * contactOnset;
				}
			}
			// Curvature warp varies by family, avoiding a globally coherent FM
			// sweep while retaining twice-per-cycle helical stress motion.
			const float warpDepth = pair == 0 ? 0.10f
				: 0.010f + 0.0025f * float(pair % 5);
			const float localStiffness = stiffnessScale
				* (1.f + warpDepth * normalizedBend)
				* ((tuningVariant == HelicalTuningVariant::DeepSwing && pair == 0)
					? (1.f - 0.58f * hardStrikeDrive) : 1.f);
			float dampingScale = 1.f;
			if (tuningVariant == HelicalTuningVariant::DeepSwing) {
				if (pair == 0) {
					dampingScale = 1.f - 0.78f * hardStrikeDrive;
				}
				else {
					const float hardT60 = DEEP_HARD_T60_SCALE[pair];
					dampingScale += hardStrikeDrive * (1.f / hardT60 - 1.f);
				}
			}
			mode.acceleration = generalizedForce
				- dampingScale * damping[index] * mode.velocity
				- localStiffness * omegaSq[index] * mode.position;
			mode.velocity += h * mode.acceleration;
			mode.position += h * mode.velocity;

			const float observedMotion = mode.acceleration
				+ RADIATION_VELOCITY_COEFFICIENT * mode.velocity;
			const float stationDepth = 0.82f + 0.16f
				* std::fabs(observerDirectionX[index]);
			const float lobedRadiation = crossingLobe * crossingLobe;
			// Deep Swing deliberately spends more of a hard hit near maximum bend.
			// Raise its omnidirectional radiation floor with the same hard-strike
			// drive so greater mechanical excitation cannot disappear merely because
			// the observer happens to be between crossing lobes.
			// Only the bending/body pairs gain persistent hard-strike radiation.
			// Upper pairs remain crossing-gated so a large bend does not become a
			// bank of continuously radiating bell partials.
			const float hardRadiationShare = pair <= 2 ? 1.f : 0.f;
			const float radiationFloor = 0.08f
				+ (tuningVariant == HelicalTuningVariant::DeepSwing
					? 0.85f * hardStrikeDrive * hardRadiationShare : 0.f);
			float observerGain = radiationFloor
				+ stationDepth * (1.f - radiationFloor) * lobedRadiation;
			observerGain *= 1.f + 0.045f * direction
				* observerDirectionY[index];
			const float hardOutputScale =
				tuningVariant == HelicalTuningVariant::DeepSwing
					? 1.f + hardStrikeDrive
						* (DEEP_HARD_OUTPUT_SCALE[pair] - 1.f)
					: 1.f;
			radiation += radiationWeight[index] * hardOutputScale
				* observerGain * observedMotion;
		}
	}
	// Recover the overall hard-strike level after the body-weighted projection.
	// This gain follows the already-shaped modal sum, so it does not restore the
	// upper bell emphasis removed above.
	if (tuningVariant == HelicalTuningVariant::DeepSwing) {
		radiation *= 1.f + 0.50f * hardStrikeDrive;
		// A harder cap hit has a stronger prompt attack even when the upper
		// structural pairs remain crossing-gated. Keep this tied to the bounded
		// strike pulse so it cannot turn into another sustained pitched voice.
		radiation += 0.10f * hardStrikeDrive * externalX;
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
	return std::isfinite(dcPreviousOutput) && std::isfinite(previousContactForce)
		&& std::isfinite(hardStrikeDrive);
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
		// Nonlinear strain-dependent stiffness hardening:
		// Produces authentic pitch descent/twang during large swings
		stiffnessScale = 1.f + 0.035f * float(strain / (1.0 + strain));
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

	const float preconditioned = processDcBlocker(radiation * 0.020f);
	const float output = 5.f * std::tanh(preconditioned * 0.55f);
	const float audibleVisualTarget = std::min(1.f, std::fabs(output) * 0.8f);
	visualAudibleEnvelope = std::max(
		audibleVisualTarget, visualAudibleEnvelope * visualEnvelopeDecay);
	const float energy = calculateEnergy();
	const float normalizedEnergy = std::min(1.f, energy / VISUAL_ENERGY_REFERENCE);
	strikeLight *= strikeLightDecay;
	hardStrikeDrive *= hardStrikeDecay;

	// The retained body contains intentionally quiet, high-Q coordinates. Do
	// not truncate them merely because their mechanical units are small.
	const bool quiet = strikePulse.remainingSubsteps == 0
		&& energy < 2.0e-12f && std::fabs(output) < 1.0e-6f;
	quietTime = quiet ? quietTime + sampleTime : 0.f;
	const bool wasSleeping = sleeping;
	if (quietTime >= 0.075f) {
		clearDynamicState();
		sleeping = true;
	}

	Frame frame;
	frame.outputVolts = output;
	// Modal coordinates are mechanical-model units; map only the retained
	// fundamental back into the panel's visual coordinate system.
	constexpr float VISUAL_DISPLACEMENT_GAIN = 450.f;
	constexpr float VISUAL_VELOCITY_GAIN = 1.625f;
	const float deepSwingVisualScale = tuningVariant == HelicalTuningVariant::DeepSwing
		? 1.f + 0.55f * hardStrikeDrive : 1.f;
	const float rawVisualDisplacement = modes[0].position
		* VISUAL_DISPLACEMENT_GAIN * visualAudibleEnvelope * deepSwingVisualScale;
	// Preserve visible low-body motion below the large-swing range instead of
	// letting the panel appear parked while the same bend mode is still audible.
	const float visualDetailGain = tuningVariant == HelicalTuningVariant::DeepSwing
		? 1.f + 8.f * hardStrikeDrive
			* (1.f - std::min(std::fabs(rawVisualDisplacement) * 0.5f, 1.f))
		: 1.f;
	constexpr float MAXIMUM_VISUAL_DISPLACEMENT = 2.75f;
	frame.displacement = std::max(-MAXIMUM_VISUAL_DISPLACEMENT, std::min(
		rawVisualDisplacement * visualDetailGain, MAXIMUM_VISUAL_DISPLACEMENT));
	frame.velocity = std::max(-1.f, std::min(
		modes[0].velocity * VISUAL_VELOCITY_GAIN * visualAudibleEnvelope, 1.f));
	frame.energy = normalizedEnergy;
	frame.strikeLight = strikeLight;
	frame.sleeping = sleeping;
	frame.enteredSleep = !wasSleeping && sleeping;
	return frame;
}

} // namespace doorstop
