#include "ReferenceSpringEngine.hpp"
#include "MathHelpers.hpp"

#include <algorithm>
#include <cmath>

namespace doorstop {
namespace {

constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_FREQUENCIES {{
	118.f, 205.f, 310.f,
	410.f, 586.f, 773.f, 1020.f, 1289.f, 1580.f, 2508.f, 3380.f, 4680.f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_DECAYS {{
	4.80f, 4.50f, 4.20f,
	4.50f, 3.60f, 3.00f, 2.40f, 2.50f, 2.10f, 1.50f, 1.00f, 0.65f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_IMPACT {{
	80.f, 145.f, 210.f,
	235.f, 270.f, 285.f, 300.f, 310.f, 275.f, 245.f, 155.f, 90.f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_CROSSING {{
	16.f, -13.f, 10.f,
	18.f, -14.f, 11.f, -9.f, 8.f, -6.f, 4.f, -2.5f, 1.5f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_OUTPUT {{
	0.75f, 0.72f, 0.65f,
	0.62f, 1.25f, 1.25f, 1.70f, 2.15f, 2.25f, 2.90f, 3.80f, 5.00f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> BASE_WARP_DEPTH {{
	0.004f, 0.006f, 0.008f,
	0.008f, 0.020f, 0.038f, 0.055f, 0.070f, 0.060f, 0.040f, 0.025f, 0.015f
}};
constexpr std::array<float, 3> TEXTURE_ALLPASS {{
	0.31f, 0.53f, 0.68f
}};
constexpr std::array<float, REFERENCE_MODE_COUNT> JUNCTION_MODE_SHAPE {{
	0.80f, -0.74f, 0.68f, -0.62f, 0.56f, -0.50f,
	0.45f, -0.40f, 0.35f, -0.30f, 0.25f, -0.20f
}};
constexpr float TEXTURE_ROUND_TRIP_HZ = 47.f;
constexpr float TEXTURE_FEEDBACK = 0.74f;

// The spring-forward probe asks whether the existing distributed path becomes
// convincing once the long-lived scalar resonances stop defining the apparent
// material. V2 promotes the successful dark/refined policy.
constexpr std::array<float, REFERENCE_MODE_COUNT>
	SPRING_FORWARD_DECAY_SCALE {{
		0.82f, 0.80f, 0.76f, 0.72f, 0.67f, 0.61f,
		0.55f, 0.49f, 0.43f, 0.37f, 0.31f, 0.26f
	}};
constexpr std::array<float, REFERENCE_MODE_COUNT>
	SPRING_FORWARD_OUTPUT_SCALE {{
		0.72f, 0.44f, 0.66f, 0.38f, 0.58f, 0.34f,
		0.50f, 0.30f, 0.43f, 0.25f, 0.34f, 0.20f
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

ReferenceSpringEngine::ReferenceSpringEngine(
	ReferenceSpringProfile selectedProfile)
	: profile(selectedProfile) {
	updateCoefficients();
	resetMotion();
}

bool ReferenceSpringEngine::usesRefinedBody() const {
	if (profile == ReferenceSpringProfile::DarkRefinedV2) {
		return true;
	}
#if defined(DOORSTOP_REFERENCE_ANALYSIS)
	return analysisVariant == ReferenceAnalysisVariant::SpringRefined
		|| analysisVariant == ReferenceAnalysisVariant::BoingRefined;
#else
	return false;
#endif
}

bool ReferenceSpringEngine::usesBoingRefinement() const {
#if defined(DOORSTOP_REFERENCE_ANALYSIS)
	return analysisVariant == ReferenceAnalysisVariant::BoingRefined;
#else
	return false;
#endif
}

bool ReferenceSpringEngine::usesDarkV2Bias() const {
	return profile == ReferenceSpringProfile::DarkRefinedV2;
}

bool ReferenceSpringEngine::usesJunctionCoupling() const {
	if (profile == ReferenceSpringProfile::DarkRefinedV2) {
		return false;
	}
#if defined(DOORSTOP_REFERENCE_ANALYSIS)
	return analysisVariant != ReferenceAnalysisVariant::SpringOnly
		&& analysisVariant != ReferenceAnalysisVariant::SpringForward
		&& analysisVariant != ReferenceAnalysisVariant::SpringRefined
		&& analysisVariant != ReferenceAnalysisVariant::BoingRefined;
#else
	return true;
#endif
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

#if defined(DOORSTOP_REFERENCE_ANALYSIS)
void ReferenceSpringEngine::setAnalysisVariant(
	ReferenceAnalysisVariant variant) {
	if (variant == analysisVariant) {
		return;
	}
	analysisVariant = variant;
	updateCoefficients();
	resetMotion();
}
#endif

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
	impactHardAlpha = 1.f - std::exp(-2.f * PI * std::min(5500.f, maxCutoff) * sampleTime);
	// Keep the strike snap clear of true sub-bass while allowing the physical
	// body's low mids through. The former 180 Hz rejection reinforced the empty
	// region between the macro flex and the first 375 Hz mode.
	impactRejectAlpha = 1.f - std::exp(-2.f * PI * 70.f * sampleTime);
	flexHighpassAlpha = 1.f - std::exp(-2.f * PI * 90.f * sampleTime);
	textureDriveDecay = safeDecay(0.008f, sampleTime);
	textureBrightnessDecay = safeDecay(0.25f, sampleTime);
	textureDelaySamples = std::max(16, std::min(
		MAX_TEXTURE_DELAY - 1,
		int(sampleRate / TEXTURE_ROUND_TRIP_HZ + 0.5f)));
	const float textureMaxCutoff = 0.45f * sampleRate;
	const bool refinedTexture = usesRefinedBody();
	const float textureSoftCutoff = refinedTexture ? 1500.f : 2400.f;
	const float textureHardCutoff = refinedTexture ? 3800.f : 6500.f;
	const float textureRejectCutoff = refinedTexture ? 190.f : 650.f;
	textureFeedbackGain = refinedTexture ? 0.885f : TEXTURE_FEEDBACK;
	textureSoftAlpha = 1.f - std::exp(-2.f * PI
		* std::min(textureSoftCutoff, textureMaxCutoff) * sampleTime);
	textureHardAlpha = 1.f - std::exp(-2.f * PI
		* std::min(textureHardCutoff, textureMaxCutoff) * sampleTime);
	textureRejectAlpha = 1.f - std::exp(
		-2.f * PI * textureRejectCutoff * sampleTime);
	textureActivityDecay = std::pow(
		textureFeedbackGain, 1.f / float(textureDelaySamples));
	// Preserve the macro bend in the audio path. The blocker remains only as a
	// slow safety stage for numerical DC rather than a bass-shaping filter.
	dcPole = std::exp(-2.f * PI * 5.f * sampleTime);
	updateWearAndSpecimen();
}

void ReferenceSpringEngine::updateWearAndSpecimen() {
	const float wear = breakIn * breakIn * (3.f - 2.f * breakIn);
	// A specimen is governed by a few correlated physical traits in addition
	// to its small per-mode irregularities. This lets one seed describe a dark,
	// heavy spring and another a bright, lightly coupled one rather than merely
	// jittering every resonance around the same spectral envelope.
	const float rawSpecimenBrightness = specimenUnit(0xa00u);
	// V2 retains meaningful specimen variation but centers it on the darker
	// steel family selected in the matched listening pass.
	const float specimenBrightness = usesDarkV2Bias()
		? clampf(-0.45f + 0.55f * rawSpecimenBrightness, -1.f, 0.10f)
		: rawSpecimenBrightness;
	const float lowerBodyCoupling =
		1.f + 0.25f * specimenUnit(0xa01u);
	const float specimenFrequencyScale =
		1.f + 0.12f * specimenUnit(0xa02u);
	const float specimenJunctionCoupling =
		1.f + 0.15f * specimenUnit(0xa03u);
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
	const float wornJunctionScale = lerpf(1.f, 0.82f, wear);
	const float junctionScale =
		specimenJunctionCoupling * wornJunctionScale;
	junctionLinearStiffness = 0.020f * baseOmegaSq * junctionScale;
	junctionCubicStiffness = 0.060f * baseOmegaSq * junctionScale;
	junctionDamping = 0.18f * junctionScale;
	if (!usesJunctionCoupling()) {
		// The previous junction experiment changed the waveform and consumed
		// measurable CPU without moving the corpus metrics. Exclude it from
		// V2 and spring-core probes so it cannot color the result.
		junctionLinearStiffness = 0.f;
		junctionCubicStiffness = 0.f;
		junctionDamping = 0.f;
	}
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
		const float modePosition =
			float(i) / float(REFERENCE_MODE_COUNT - 1);
		const float signedTilt = 2.f * modePosition - 1.f;
		const float spectralTilt =
			clampf(1.f + 0.18f * specimenBrightness * signedTilt, 0.80f, 1.20f);
		const float decayTilt =
			clampf(1.f + 0.15f * specimenBrightness * signedTilt, 0.82f, 1.18f);
		const float wearFrequency = lerpf(1.f, 0.94f - 0.004f * float(i), wear);
		restingFrequencyHz[i] = BASE_FREQUENCIES[i]
			* (1.f + 0.03f * specimenUnit(0x200u + tag))
			* specimenFrequencyScale
			* wearFrequency;
		decayT60Seconds[i] = BASE_DECAYS[i]
			* (1.f + 0.12f * specimenUnit(0x300u + tag))
			* decayTilt
			* lerpf(1.f, 1.18f, wear);
		impactExcitation[i] = BASE_IMPACT[i]
			* (1.f + 0.08f * specimenUnit(0x400u + tag));
		crossingExcitation[i] = BASE_CROSSING[i]
			* (1.f + 0.10f * specimenUnit(0x500u + tag));
		outputGain[i] = BASE_OUTPUT[i]
			* (1.f + 0.10f * specimenUnit(0x600u + tag))
			* spectralTilt
			* (i < 4 ? lowerBodyCoupling : 1.f);
#if defined(DOORSTOP_REFERENCE_ANALYSIS)
		if (analysisVariant == ReferenceAnalysisVariant::SpringForward
			|| usesRefinedBody()) {
			decayT60Seconds[i] *= SPRING_FORWARD_DECAY_SCALE[i];
			outputGain[i] *= SPRING_FORWARD_OUTPUT_SCALE[i];
		}
#else
		if (usesRefinedBody()) {
			decayT60Seconds[i] *= SPRING_FORWARD_DECAY_SCALE[i];
			outputGain[i] *= SPRING_FORWARD_OUTPUT_SCALE[i];
		}
#endif
		directionTilt[i] = specimenUnit(0x700u + tag)
			* lerpf(0.03f, 0.10f, modePosition);
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
	junctionForce = 0.f;
	junctionRelativeDisplacement = 0.f;
	junctionModalDisplacement = 0.f;
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
	flexHighpassLowpass.fill(0.f);
	textureDelay.fill(0.f);
	textureAllpassState.fill(0.f);
	textureWriteIndex = 0;
	textureDriveEnvelope = 0.f;
	textureBrightness = 0.f;
	textureDriveLowpass = 0.f;
	textureFeedbackLowpass = 0.f;
	texturePreviousOutput = 0.f;
	textureLowReject = 0.f;
	textureActivity = 0.f;
	textureActive = false;
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
	textureDriveEnvelope = std::min(textureDriveEnvelope + shaped, 2.f);
	textureBrightness = std::max(textureBrightness, shaped);
	textureActivity = std::min(textureActivity + shaped, 2.f);
	textureActive = true;
	strikeLightEnvelope = std::max(strikeLightEnvelope, shaped);
	quietTime = 0.f;
	sleeping = false;
}

bool ReferenceSpringEngine::processFlexAndCrossing() {
	previousDisplacement = displacement;
	const float x2 = displacement * displacement;
	const float restoring = baseOmegaSq * displacement
		+ nonlinearStiffness * baseOmegaSq * displacement * x2;
	float couplingForce = 0.f;
	if (usesJunctionCoupling()) {
		couplingForce = updateJunctionCouplingForce();
	}
	else {
		junctionForce = 0.f;
		junctionRelativeDisplacement = 0.f;
		junctionModalDisplacement = 0.f;
	}
	acceleration =
		-restoring - springDamping * springVelocity - couplingForce;
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

float ReferenceSpringEngine::updateJunctionCouplingForce() {
	float modalDisplacement = 0.f;
	float modalVelocity = 0.f;
	for (int i = 0; i < REFERENCE_MODE_COUNT; ++i) {
		const float shape = JUNCTION_MODE_SHAPE[i];
		modalDisplacement += shape * modes[i].position;
		modalVelocity += shape * modes[i].velocity;
	}

	junctionModalDisplacement = modalDisplacement;
	junctionRelativeDisplacement = displacement - junctionModalDisplacement;
	const float relativeVelocity = springVelocity - modalVelocity;
	const float r2 =
		junctionRelativeDisplacement * junctionRelativeDisplacement;
	const float elasticForce =
		junctionLinearStiffness * junctionRelativeDisplacement
		+ junctionCubicStiffness * junctionRelativeDisplacement * r2;
	const float dampingForce = junctionDamping * relativeVelocity;
	const float forceLimit = 0.18f * 24000.f;
	junctionForce = clampf(
		elasticForce + dampingForce, -forceLimit, forceLimit);
	return junctionForce;
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
	// A crossing only whispers into the texture. It must articulate stored
	// dispersion, not replay the broadband initial strike.
	const float crossingTextureTransfer =
		usesRefinedBody() ? 0.065f : 0.04f;
	textureDriveEnvelope = std::max(
		textureDriveEnvelope, crossingTextureTransfer * normalizedSpeed);
	textureActivity = std::min(
		textureActivity + crossingTextureTransfer * normalizedSpeed, 2.f);
	textureActive = true;
}

float ReferenceSpringEngine::processModes() {
	const float energy = clampf(smoothedEnergy, 0.f, 1.f);
	float output = 0.f;
	for (int i = 0; i < REFERENCE_MODE_COUNT; ++i) {
		// Measured ridges settle by different amounts. A small mode-specific
		// warp avoids the synthetic global pitch dive of the previous model.
		// The boing candidate keeps the existing correlated energy warp but
		// makes its downward settling audible. This is still one multiply per
		// mode, with no new per-sample transcendental work.
		const float warpScale = usesBoingRefinement() ? 2.4f : 1.f;
		const float pitchWarp =
			1.f + frequencyWarpDepth[i] * energy * warpScale;
		const float frequency = std::min(
			restingFrequencyHz[i] * pitchWarp,
			0.18f * sampleRate);
		const float omega = 2.f * PI * frequency;
		const float gamma = LN_1000 / std::max(decayT60Seconds[i], 1e-4f);
		Mode& mode = modes[i];
		const float modeAcceleration = -omega * omega * mode.position
			- 2.f * gamma * mode.velocity
			+ JUNCTION_MODE_SHAPE[i] * junctionForce;
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
	float filtered =
		0.12f * normalizedVelocity + 0.055f * normalizedAcceleration;
	// The bend conducts the audible body but is not itself the bass voice.
	// Two inexpensive one-pole stages keep tactile edge while suppressing the
	// isolated 21 Hz shelf identified during reference fitting.
	for (float& lowpass : flexHighpassLowpass) {
		lowpass += (filtered - lowpass) * flexHighpassAlpha;
		filtered -= lowpass;
	}
	return filtered;
}

float ReferenceSpringEngine::processDispersiveTexture() {
	if (!textureActive) {
		return 0.f;
	}

	const float noise = float(nextNoiseRandom() & 0x00ffffffu)
		/ float(0x00800000u) - 1.f;
	const float brightness = clampf(textureBrightness, 0.f, 1.f);
	const float driveAlpha = lerpf(textureSoftAlpha, textureHardAlpha, brightness);
	textureDriveLowpass += (noise - textureDriveLowpass) * driveAlpha;
	const float drive = textureDriveLowpass * textureDriveEnvelope
		* (0.16f + 0.20f * brightness);

	float delayed = 0.f;
	if (usesBoingRefinement()) {
		// A small energy-correlated delay sweep lets the distributed texture
		// settle downward with the modes. Linear interpolation avoids the
		// zipper noise of jumping between integer delay taps.
		const float energy = clampf(smoothedEnergy, 0.f, 1.f);
		const float sweptDelay = std::max(
			16.f, float(textureDelaySamples) * (1.f - 0.055f * energy));
		const int delayWhole = int(sweptDelay);
		const float delayFraction = sweptDelay - float(delayWhole);
		int newerIndex = textureWriteIndex - delayWhole;
		if (newerIndex < 0) {
			newerIndex += MAX_TEXTURE_DELAY;
		}
		int olderIndex = newerIndex - 1;
		if (olderIndex < 0) {
			olderIndex += MAX_TEXTURE_DELAY;
		}
		delayed = lerpf(
			textureDelay[newerIndex], textureDelay[olderIndex], delayFraction);
	}
	else {
		int readIndex = textureWriteIndex - textureDelaySamples;
		if (readIndex < 0) {
			readIndex += MAX_TEXTURE_DELAY;
		}
		delayed = textureDelay[readIndex];
	}
	textureFeedbackLowpass +=
		(delayed - textureFeedbackLowpass) * driveAlpha;

	float dispersed = textureFeedbackLowpass;
	for (int i = 0; i < int(textureAllpassState.size()); ++i) {
		const float coefficient = TEXTURE_ALLPASS[i];
		const float output =
			-coefficient * dispersed + textureAllpassState[i];
		textureAllpassState[i] = dispersed + coefficient * output;
		dispersed = output;
	}

	textureDelay[textureWriteIndex] = clampf(
		drive - dispersed * textureFeedbackGain, -2.f, 2.f);
	if (++textureWriteIndex >= MAX_TEXTURE_DELAY) {
		textureWriteIndex = 0;
	}
	textureDriveEnvelope *= textureDriveDecay;
	textureBrightness *= textureBrightnessDecay;
	textureActivity *= textureActivityDecay;

	const float radiation = delayed - 0.30f * texturePreviousOutput;
	texturePreviousOutput = delayed;
	textureLowReject +=
		(radiation - textureLowReject) * textureRejectAlpha;
	return radiation - textureLowReject;
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
		|| !std::isfinite(mountVelocity)
		|| !std::isfinite(textureDriveEnvelope)
		|| !std::isfinite(textureBrightness)
		|| !std::isfinite(textureDriveLowpass)
		|| !std::isfinite(textureFeedbackLowpass)
		|| !std::isfinite(texturePreviousOutput)
		|| !std::isfinite(textureLowReject)
		|| !std::isfinite(textureActivity)
		|| !std::isfinite(junctionForce)
		|| !std::isfinite(junctionRelativeDisplacement)
		|| !std::isfinite(junctionModalDisplacement)
		|| !std::isfinite(strikeLightEnvelope)
		|| !std::isfinite(dcPreviousInput) || !std::isfinite(dcPreviousOutput)) {
		return false;
	}
	for (const Mode& mode : modes) {
		if (!std::isfinite(mode.position) || !std::isfinite(mode.velocity)) {
			return false;
		}
	}
	for (float state : textureAllpassState) {
		if (!std::isfinite(state)) {
			return false;
		}
	}
	for (float state : flexHighpassLowpass) {
		if (!std::isfinite(state)) {
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
		&& textureDriveEnvelope < 1e-5f
		&& textureActivity < 1e-5f
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
	const float texture = processDispersiveTexture();
	// The macro bend conducts the sound but should not form a separate sub-bass
	// shelf. Give the broadband strike more responsibility for the initial body;
	// the sustained 100--350 Hz bridge is supplied by the lower-body stage.
#if defined(DOORSTOP_REFERENCE_ANALYSIS)
	float signal = 0.f;
	switch (analysisVariant) {
		case ReferenceAnalysisVariant::SpringOnly:
			signal = impact * 0.60f + mount * 0.16f + flex * 0.30f
				+ texture * 1.80f * radiationGate;
			break;
		case ReferenceAnalysisVariant::ModesOnly:
			signal = modal * 1.90f * radiationGate;
			break;
		case ReferenceAnalysisVariant::SpringForward:
			signal = impact * 0.60f + mount * 0.16f + flex * 0.30f
				+ (modal * 0.82f + texture * 1.80f) * radiationGate;
			break;
		case ReferenceAnalysisVariant::SpringRefined: {
			// Preserve the iconic double-sided macro crossing pulse. The
			// distributed body now retains low/mid energy long enough for this
			// narrower gate to reveal several boings from one strike.
			const float refinedPulse =
				radiationEnvelope * radiationEnvelope;
			const float refinedGate = (0.035f
				+ 0.965f * refinedPulse) * directionalRadiation;
			signal = impact * 0.54f + mount * 0.16f + flex * 0.30f
				+ (modal * 0.78f + texture * 1.62f) * refinedGate;
			break;
		}
		case ReferenceAnalysisVariant::BoingRefined: {
			// Broader shoulders keep each bidirectional radiation window from
			// chopping a stable carrier into a sequence of metallic twangs.
			// Pitch motion is supplied inside the modal and texture stages.
			const float roundedPulse = radiationEnvelope
				* (0.35f + 0.65f * radiationEnvelope);
			const float roundedGate = (0.045f
				+ 0.955f * roundedPulse) * directionalRadiation;
			signal = impact * 0.43f + mount * 0.16f + flex * 0.30f
				+ (modal * 0.78f + texture * 1.62f) * roundedGate;
			break;
		}
		case ReferenceAnalysisVariant::Current:
		default:
			if (usesRefinedBody()) {
				const float refinedPulse =
					radiationEnvelope * radiationEnvelope;
				const float refinedGate = (0.035f
					+ 0.965f * refinedPulse) * directionalRadiation;
				signal = impact * 0.54f + mount * 0.16f + flex * 0.30f
					+ (modal * 0.78f + texture * 1.62f) * refinedGate;
			}
			else {
				signal = impact * 0.60f + mount * 0.16f + flex * 0.30f
					+ (modal * 1.90f + texture * 1.25f) * radiationGate;
			}
			break;
	}
#else
	float signal = 0.f;
	if (usesRefinedBody()) {
		const float refinedPulse =
			radiationEnvelope * radiationEnvelope;
		const float refinedGate = (0.035f
			+ 0.965f * refinedPulse) * directionalRadiation;
		signal = impact * 0.54f + mount * 0.16f + flex * 0.30f
			+ (modal * 0.78f + texture * 1.62f) * refinedGate;
	}
	else {
		signal = impact * 0.60f + mount * 0.16f + flex * 0.30f
			+ (modal * 1.90f + texture * 1.25f) * radiationGate;
	}
#endif
	signal = processDcBlocker(signal);
	// Keep the safety saturation from turning the strengthened lower modes
	// into a synthetic spray of upper harmonics. The RMS-matched V2 audition
	// required about 5x makeup after demoting the modal bank; apply that makeup
	// inside the existing bounded stage for a useful modular output level.
	const float outputDrive = usesDarkV2Bias() ? 10.5f : 2.2f;
	const float outputVolts =
		5.f * levi_math::tanhAudio(signal * outputDrive);
	strikeLightEnvelope *= strikeLightDecay;

	diagnostics.radiationGate = radiationGate;
	diagnostics.normalizedModalEnergy = normalizedModalEnergy();
	diagnostics.dispersiveTexture = texture;
	diagnostics.junctionForce = junctionForce;
	diagnostics.junctionRelativeDisplacement =
		junctionRelativeDisplacement;
	diagnostics.junctionModalDisplacement =
		junctionModalDisplacement;

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
