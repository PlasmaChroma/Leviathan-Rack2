#include "DoorstopContactHelixEngine.hpp"

#include <algorithm>
#include <cmath>

namespace doorstop {
namespace {
constexpr float PI = 3.14159265358979323846f;
constexpr int SUBSTEPS = 4;
float clamp01(float x) { return std::max(0.f, std::min(x, 1.f)); }
}

void ContactHelixEngine::updateCoefficients() {
	const float trait = float((specimenSeed ^ (specimenSeed >> 16)) & 1023u) / 1023.f - 0.5f;
	for (int i = 0; i < COORDINATE_COUNT; ++i) {
		const float rank = float(i);
		const float hz = (i < 2 ? 18.f + 4.f * rank : 82.f + 30.f * rank + 1.4f * rank * rank)
			* (1.f + trait * 0.035f);
		omega[i] = 2.f * PI * hz;
		// V4's paired low coordinates are the perceptible bend/return gesture,
		// not merely a control envelope. Keep them long-lived and let the fixed
		// observer hear their alternating load of the closed coil.
		damping[i] = (i < 2 ? 0.34f + 0.05f * rank : 1.35f + 0.060f * rank)
			* (1.f + 0.28f * breakIn);
		drive[i] = (i < 2 ? 1.80f : 0.30f / (1.f + 0.07f * rank))
			* ((i & 1) ? -1.f : 1.f);
		if (i < 2) observe[i] = 0.16f * ((i & 1) ? 1.f : -1.f);
		else if (i == 2) observe[i] = 2.15f;
		else if (i == 3) observe[i] = 1.15f;
		else observe[i] = (0.58f / (1.f + 0.060f * rank))
			* ((i % 3) ? 1.f : -1.f);
	}
}

void ContactHelixEngine::setSampleRate(float rate) {
	if (std::isfinite(rate) && rate >= 1000.f) sampleRate = rate;
}

void ContactHelixEngine::setBreakIn(float amount) {
	if (!std::isfinite(amount)) return;
	breakIn = clamp01(amount);
	updateCoefficients();
}

void ContactHelixEngine::setSpecimenSeed(std::uint32_t seed) {
	specimenSeed = seed ? seed : 1u;
	updateCoefficients();
}

void ContactHelixEngine::setRequestedModelDataRevision(int revision) {
	requestedModelDataRevision = revision > 0 ? revision : MODEL_DATA_REVISION;
	diagnostics.modelRevisionMismatch = requestedModelDataRevision != MODEL_DATA_REVISION;
}

void ContactHelixEngine::setLiveTuning(float excitation, float bend, float twang,
	float contact, float decay, float output) {
	auto safeScale = [](float value, float minimum, float maximum) {
		return std::isfinite(value) ? std::max(minimum, std::min(value, maximum)) : 1.f;
	};
	excitationScale = safeScale(excitation, 0.25f, 2.f);
	bendScale = safeScale(bend, 0.25f, 2.f);
	fundamentalScale = safeScale(twang, 0.f, 2.f);
	contactScale = safeScale(contact, 0.25f, 2.f);
	decayScale = safeScale(decay, 0.4f, 2.5f);
	outputScale = safeScale(output, 0.25f, 2.f);
}

void ContactHelixEngine::setAdvancedTuning(float pitch, float dispersion,
	float sweep, float metal, float contactLoss, float gap, float capMass,
	float mountCompliance, float bendDecay) {
	auto safeScale = [](float value, float minimum, float maximum, float fallback) {
		return std::isfinite(value) ? std::max(minimum, std::min(value, maximum)) : fallback;
	};
	pitchScale = safeScale(pitch, 0.5f, 1.5f, 1.f);
	dispersionScale = safeScale(dispersion, 0.5f, 1.5f, 1.f);
	sweepScale = safeScale(sweep, 0.f, 2.f, 1.f);
	metalScale = safeScale(metal, 0.f, 2.f, 1.f);
	contactLossScale = safeScale(contactLoss, 0.f, 2.f, 1.f);
	gapControl = safeScale(gap, -1.f, 1.f, 0.f);
	capMassScale = safeScale(capMass, 0.5f, 2.f, 1.f);
	mountComplianceScale = safeScale(mountCompliance, 0.5f, 2.f, 1.f);
	bendDecayScale = safeScale(bendDecay, 0.4f, 2.5f, 1.f);
}

void ContactHelixEngine::setMotionTuning(float bendRate, float transfer,
	float radiation) {
	auto safeScale = [](float value, float minimum, float maximum, float fallback) {
		return std::isfinite(value) ? std::max(minimum, std::min(value, maximum)) : fallback;
	};
	bendRateScale = safeScale(bendRate, 0.5f, 1.5f, 1.f);
	transferScale = safeScale(transfer, 0.f, 2.f, 1.f);
	radiationControl = safeScale(radiation, -1.f, 1.f, 0.f);
}

void ContactHelixEngine::setV3BodyTuning(float pairing, float reaction,
	float lobes, float attack, float flickTime, float midBody, float v3Pitch) {
	auto safeScale = [](float value, float minimum, float maximum, float fallback) {
		return std::isfinite(value) ? std::max(minimum, std::min(value, maximum)) : fallback;
	};
	pairingScale = safeScale(pairing, 0.f, 1.f, contact_helix_defaults::PAIRING);
	reactionScale = safeScale(reaction, 0.f, 2.f, contact_helix_defaults::REACTION);
	lobeScale = safeScale(lobes, 0.f, 2.f, contact_helix_defaults::LOBES);
	attackScale = safeScale(attack, 0.f, 2.f, contact_helix_defaults::ATTACK);
	flickTimeScale = safeScale(flickTime, 0.3f, 2.f, contact_helix_defaults::FLICK_TIME);
	midBodyScale = safeScale(midBody, 0.f, 2.f, contact_helix_defaults::MID_BODY);
	v3PitchScale = safeScale(v3Pitch, 0.5f, 1.5f, contact_helix_defaults::V3_PITCH);
}

void ContactHelixEngine::resetMotion() {
	q.fill(0.f); v.fill(0.f); pulses.fill(Pulse {});
	capPosition = capVelocity = mountPosition = mountVelocity = 0.f;
	reactionPosition = reactionVelocity = 0.f;
	dcInput = dcOutput = strikeLight = quietTime = 0.f;
	sleeping = true;
	const bool mismatch = diagnostics.modelRevisionMismatch;
	diagnostics = ContactHelixDiagnostics {};
	diagnostics.modelRevisionMismatch = mismatch;
}

void ContactHelixEngine::restoreFactoryFresh() { breakIn = 0.f; updateCoefficients(); resetMotion(); }
void ContactHelixEngine::reset() { breakIn = 0.f; breakInLocked = false; requestedModelDataRevision = MODEL_DATA_REVISION; updateCoefficients(); resetMotion(); }

void ContactHelixEngine::strike(float velocity) {
	if (!std::isfinite(velocity) || velocity == 0.f) return;
	Pulse* slot = nullptr;
	for (auto& pulse : pulses) if (pulse.remaining == 0) { slot = &pulse; break; }
	if (!slot) { ++diagnostics.rejectedStrikes; return; }
	const float bounded = std::max(-1.f, std::min(velocity, 1.f));
	slot->total = std::max(8,
		int(0.0045f * flickTimeScale * sampleRate * SUBSTEPS));
	slot->remaining = slot->total;
	// A real hard flick stores disproportionately more bend energy than a
	// moderate release. Keep the ordinary strike near its previous impulse but
	// open up the final part of the velocity range for large arc excursions.
	const float magnitude = std::fabs(bounded);
	// Spread useful action across the entire 0--10 V velocity range. The former
	// cubic curve reserved most of the impulse for its final few volts. This
	// bounded blend retains a strong hard hit while making 1--5 V musically
	// consequential and preserving an exact zero-input no-op.
	const float shapedMagnitude = 0.80f * magnitude
		+ 0.80f * std::sqrt(magnitude);
	slot->amplitude = std::copysign(520.f * excitationScale * shapedMagnitude, bounded);
	strikeLight = std::max(strikeLight, std::fabs(bounded));
	sleeping = false;
	quietTime = 0.f;
	if (!breakInLocked) { breakIn = std::min(1.f, breakIn + 0.0015f * std::fabs(bounded)); updateCoefficients(); }
}

float ContactHelixEngine::contactJacobian(int c, int i) const {
	const int a = 2 + c;
	const int b = 2 + ((c + 7) % 28);
	if (i == a) return 0.00034f;
	if (i == b) return -0.00027f;
	if (i == 0) return (c & 1) ? 0.00010f : -0.00010f;
	if (i == 1) return (c & 2) ? 0.00007f : -0.00007f;
	return 0.f;
}

float ContactHelixEngine::contactGap(int c) const {
	// Zero-load touching equilibrium; bend and wire coordinates open or compress
	// unique adjacent-turn patches. Gap and velocity use this same Jacobian.
	float gap = gapControl * 0.000020f;
	for (int i = 0; i < COORDINATE_COUNT; ++i) gap += contactJacobian(c, i) * q[i];
	return gap;
}

float ContactHelixEngine::effectiveOmegaSquared(int coordinate) const {
	float effectiveOmega = omega[coordinate];
	if (coordinate < 2) effectiveOmega *= bendRateScale;
	else {
		const float rank = float(coordinate - 2);
		const float dispersion = std::max(0.55f,
			1.f + (dispersionScale - 1.f) * 0.022f * rank);
		effectiveOmega *= pitchScale * dispersion;
		const int pairStart = 2 + 2 * ((coordinate - 2) / 2);
		const int pairOther = pairStart + 1;
		if (pairOther < COORDINATE_COUNT) {
			// The imported V3-like body has its own pitch reference. Calibrating it
			// to the established V4 baseline preserves the current 1.00x sound,
			// while live V4 PITCH can now move the unpaired helix independently.
			const float pairedCenter = 0.5f * (omega[pairStart] + omega[pairOther])
				* contact_helix_defaults::PITCH * dispersion * v3PitchScale;
			const float pairedFrequency = pairedCenter
				* (1.f + ((coordinate & 1) ? 0.006f : -0.006f));
			effectiveOmega += pairingScale * (pairedFrequency - effectiveOmega);
		}
	}
	return effectiveOmega * effectiveOmega;
}

float ContactHelixEngine::processSubstep(float h) {
	float external = 0.f;
	for (auto& pulse : pulses) {
		if (pulse.remaining <= 0) continue;
		const float phase = float(pulse.total - pulse.remaining + 1) / float(pulse.total + 1);
		const float force = pulse.amplitude * std::sin(PI * phase);
		external += force;
		diagnostics.appliedImpulse += force * h;
		--pulse.remaining;
	}

	std::array<float, COORDINATE_COUNT> force {};
	// Conservative bend-to-wire coupling: deflection raises the audible body's
	// stiffness, then relaxes it as the spring returns. The matching reaction on
	// the two bend coordinates keeps this a shared potential rather than an
	// unaccounted pitch envelope.
	const float bendTwangCoupling = 350.f * sweepScale;
	const float bendSquared = q[0] * q[0] + q[1] * q[1];
	const float wireStiffnessScale = 1.f + bendTwangCoupling * bendSquared;
	float wireEnergyGradientScale = 0.f;
	for (int i = 0; i < COORDINATE_COUNT; ++i) {
		const float omegaSquared = effectiveOmegaSquared(i);
		const float stiffnessScale = i >= 2 ? wireStiffnessScale : 1.f;
		const float driveScale = i < 2 ? bendScale : 1.f;
		const float activeDecayScale = i < 2 ? bendDecayScale : decayScale;
		force[i] = drive[i] * driveScale * external
			- omegaSquared * stiffnessScale * q[i]
			- 2.f * damping[i] * v[i] / activeDecayScale;
		if (i >= 2) wireEnergyGradientScale += omegaSquared * q[i] * q[i];
	}
	force[0] -= bendTwangCoupling * q[0] * wireEnergyGradientScale;
	force[1] -= bendTwangCoupling * q[1] * wireEnergyGradientScale;
	// A conservative attachment-like coupling transfers energy between the
	// large bending arc and the first audible wire coordinate independently of
	// the bend-dependent stiffness sweep above.
	const float projectedBend = q[0] - 0.55f * q[1];
	const float transferAlpha = 0.18f;
	const float transferStiffness = effectiveOmegaSquared(2) * 0.12f * transferScale;
	const float transferExtension = q[2] - transferAlpha * projectedBend;
	force[2] -= transferStiffness * transferExtension;
	force[0] += transferAlpha * transferStiffness * transferExtension;
	force[1] -= 0.55f * transferAlpha * transferStiffness * transferExtension;

	// Explicit low reaction/body coordinate, conservatively attached to the
	// projected bend. This fills the missing ~60 Hz rung without launching an
	// independently triggered oscillator.
	const float reactionOmega = 2.f * PI * 62.f * v3PitchScale;
	const float reactionCoupling = reactionOmega * reactionOmega * 0.10f * reactionScale;
	const float reactionExtension = reactionPosition - 0.16f * projectedBend;
	const float reactionAcceleration = -reactionOmega * reactionOmega * reactionPosition
		- 2.f * 2.8f * reactionVelocity - reactionCoupling * reactionExtension;
	force[0] += 0.16f * reactionCoupling * reactionExtension;
	force[1] -= 0.55f * 0.16f * reactionCoupling * reactionExtension;

	diagnostics.activeContacts = 0;
	diagnostics.minimumGap = 1.f;
	diagnostics.maximumContactForce = 0.f;
	float contactEnergy = 0.f;
	for (int c = 0; c < CONTACT_COUNT; ++c) {
		const float gap = contactGap(c);
		diagnostics.minimumGap = std::min(diagnostics.minimumGap, gap);
		if (gap >= 0.f) continue;
		float gapVelocity = 0.f;
		for (int i = 0; i < COORDINATE_COUNT; ++i) gapVelocity += contactJacobian(c, i) * v[i];
		const float penetration = -gap;
		const float elastic = 3.2e7f * contactScale * penetration;
		const float dissipative = std::max(0.f,
			-95.f * contactLossScale * std::sqrt(contactScale) * gapVelocity);
		const float normalForce = elastic + dissipative;
		++diagnostics.activeContacts;
		diagnostics.maximumContactForce = std::max(diagnostics.maximumContactForce, normalForce);
		contactEnergy += 0.5f * 3.2e7f * contactScale * penetration * penetration;
		diagnostics.dissipatedWork += dissipative * (-gapVelocity) * h;
		for (int i = 0; i < COORDINATE_COUNT; ++i) force[i] += contactJacobian(c, i) * normalForce;
	}
	diagnostics.contactEnergy = contactEnergy;

	// Explicit cap and compliant mount remain in the same solve through equal
	// and opposite attachment forces.
	const float tip = q[0] + 0.35f * q[1];
	const float attach = 1900.f * (capPosition - tip) + 7.f * (capVelocity - v[0]);
	force[0] += attach;
	capVelocity += h * (external - attach - 4.f * capVelocity)
		/ (0.018f * capMassScale);
	capPosition += h * capVelocity;
	const float mountStiffnessScale = 1.f / mountComplianceScale;
	const float mountForce = 4200.f * mountStiffnessScale
		* (mountPosition - 0.12f * q[1])
		+ 18.f * std::sqrt(mountStiffnessScale)
		* (mountVelocity - 0.12f * v[1]);
	force[1] += 0.12f * mountForce;
	mountVelocity += h * (-mountForce - 55.f * mountStiffnessScale * mountPosition
		- 1.8f * mountVelocity) / 0.06f;
	mountPosition += h * mountVelocity;

	const float projectedBendVelocity = v[0] - 0.55f * v[1];
	const float turningActivity = std::min(1.f, std::fabs(projectedBend) * 22.f);
	const float crossingActivity = std::min(1.f, std::fabs(projectedBendVelocity) * 0.035f);
	float observation = 0.f;
	for (int i = 0; i < COORDINATE_COUNT; ++i) {
		v[i] += h * force[i];
		q[i] += h * v[i];
		const float observerScale = (i == 2 || i == 3)
			? fundamentalScale : (i >= 4 ? metalScale : 1.f);
		const float midScale = i >= 4 && i <= 13 ? midBodyScale : 1.f;
		const float bandPhase = ((i / 2) & 1) ? turningActivity : crossingActivity;
		const float lobeGain = 1.f + lobeScale * (0.35f + 1.30f * bandPhase - 1.f);
		const float accelerationObservation = force[i] / (2.f * PI * 140.f);
		observation += observe[i] * observerScale * midScale * lobeGain
			* (v[i] + attackScale * accelerationObservation);
	}
	reactionVelocity += h * reactionAcceleration;
	reactionPosition += h * reactionVelocity;
	observation += reactionScale * (0.24f * reactionVelocity
		+ attackScale * 0.24f * reactionAcceleration / (2.f * PI * 140.f));
	const float radiationActivity = radiationControl >= 0.f
		? crossingActivity : turningActivity;
	const float radiationDepth = std::fabs(radiationControl);
	observation *= 1.f + radiationDepth * (1.35f * radiationActivity - 0.45f);
	return observation;
}

float ContactHelixEngine::calculateEnergy() const {
	const double mountStiffnessScale = 1.0 / double(mountComplianceScale);
	double e = 0.5 * (capVelocity * capVelocity * 0.018 * capMassScale
		+ mountVelocity * mountVelocity * 0.06
		+ 55.0 * mountStiffnessScale * mountPosition * mountPosition);
	const double bendTwangCoupling = 350.0 * double(sweepScale);
	const double wireStiffnessScale = 1.0 + bendTwangCoupling
		* (double(q[0]) * q[0] + double(q[1]) * q[1]);
	for (int i = 0; i < COORDINATE_COUNT; ++i) {
		const double stiffnessScale = i >= 2 ? wireStiffnessScale : 1.0;
		e += 0.5 * (double(v[i]) * v[i]
			+ double(effectiveOmegaSquared(i)) * stiffnessScale * q[i] * q[i]);
	}
	const double projectedBend = double(q[0]) - 0.55 * double(q[1]);
	const double transferAlpha = 0.18;
	const double transferStiffness = double(effectiveOmegaSquared(2)) * 0.12 * transferScale;
	const double transferExtension = double(q[2]) - transferAlpha * projectedBend;
	e += 0.5 * transferStiffness * transferExtension * transferExtension;
	const double reactionOmega = 2.0 * double(PI) * 62.0 * double(v3PitchScale);
	const double reactionExtension = double(reactionPosition) - 0.16 * projectedBend;
	const double reactionCoupling = reactionOmega * reactionOmega * 0.10 * reactionScale;
	e += 0.5 * double(reactionVelocity) * reactionVelocity
		+ 0.5 * reactionOmega * reactionOmega * reactionPosition * reactionPosition
		+ 0.5 * reactionCoupling * reactionExtension * reactionExtension;
	return float(e) + diagnostics.contactEnergy;
}

bool ContactHelixEngine::finiteState() const {
	if (!std::isfinite(capPosition) || !std::isfinite(capVelocity)
		|| !std::isfinite(reactionPosition) || !std::isfinite(reactionVelocity)) return false;
	for (int i = 0; i < COORDINATE_COUNT; ++i) if (!std::isfinite(q[i]) || !std::isfinite(v[i]) || std::fabs(q[i]) > 100.f) return false;
	return true;
}

Frame ContactHelixEngine::process(float requestedSampleTime) {
	Frame frame;
	if (sleeping) return frame;
	const float hostTime = std::isfinite(requestedSampleTime) && requestedSampleTime > 0.f ? requestedSampleTime : 1.f / sampleRate;
	const float h = hostTime / float(SUBSTEPS);
	float raw = 0.f;
	for (int s = 0; s < SUBSTEPS; ++s) raw += processSubstep(h);
	if (!finiteState()) {
		const std::uint32_t recoveries = diagnostics.recoveries + 1u;
		resetMotion();
		diagnostics.recoveries = recoveries;
		return frame;
	}
	raw *= 0.25f;
	const float pole = std::exp(-2.f * PI * 12.f * hostTime);
	const float dc = raw - dcInput + pole * dcOutput;
	dcInput = raw; dcOutput = dc;
	// Mechanical coordinates use compact mass-normalized units. These gains are
	// observation/display calibration only: they do not feed back into contact,
	// energy, damping, or the cap/mount solve.
	frame.outputVolts = 5.f * std::tanh(0.60f * outputScale * dc);
	const float projectedBend = q[0] - 0.55f * q[1];
	const float projectedBendVelocity = v[0] - 0.55f * v[1];
	frame.displacement = std::max(-2.75f, std::min(2.75f,
		projectedBend * 30.f + capPosition * 5.0f));
	frame.velocity = std::max(-1.f, std::min(1.f, projectedBendVelocity * 0.075f));
	diagnostics.bendMagnitude = std::sqrt(q[0] * q[0] + q[1] * q[1]);
	diagnostics.maximumBendMagnitude = std::max(
		diagnostics.maximumBendMagnitude, diagnostics.bendMagnitude);
	diagnostics.mechanicalEnergy = calculateEnergy();
	frame.energy = clamp01(diagnostics.mechanicalEnergy * 0.002f);
	frame.visualActivity = clamp01(std::fabs(frame.outputVolts) * 0.2f);
	frame.strikeLight = strikeLight;
	strikeLight *= std::exp(-hostTime / 0.075f);
	const bool pulsesActive = std::any_of(pulses.begin(), pulses.end(), [](const Pulse& p) { return p.remaining > 0; });
	if (!pulsesActive && diagnostics.mechanicalEnergy < 1e-7f && std::fabs(frame.outputVolts) < 1e-4f) quietTime += hostTime; else quietTime = 0.f;
	if (quietTime > 0.08f) { resetMotion(); frame.sleeping = true; frame.enteredSleep = true; return frame; }
	frame.sleeping = false;
	return frame;
}

} // namespace doorstop
