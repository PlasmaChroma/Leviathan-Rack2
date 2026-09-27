#pragma once

#include "DoorstopEngine.hpp"

#include <array>
#include <cstdint>

namespace doorstop {

namespace contact_helix_defaults {
constexpr float EXCITATION = 1.20f;
constexpr float BEND = 1.28f;
constexpr float BEND_DECAY = 0.45f;
constexpr float CAP_MASS = 0.82f;
constexpr float MOUNT_COMPLIANCE = 1.f;
constexpr float PITCH = 0.77f;
constexpr float DISPERSION = 1.17f;
constexpr float FUNDAMENTAL = 0.76f;
constexpr float METAL = 1.28f;
constexpr float SWEEP = 0.51f;
constexpr float GAP = -0.08f;
constexpr float CONTACT = 0.97f;
constexpr float CONTACT_LOSS = 0.92f;
constexpr float BODY_DECAY = 0.64f;
constexpr float OUTPUT = 1.f;
constexpr float BEND_RATE = 1.16f;
constexpr float TRANSFER = 0.51f;
constexpr float RADIATION = 0.80f;
}

struct ContactHelixDiagnostics {
	static constexpr int MODEL_DATA_REVISION = 1;
	float mechanicalEnergy = 0.f;
	float contactEnergy = 0.f;
	float dissipatedWork = 0.f;
	float appliedImpulse = 0.f;
	float minimumGap = 0.f;
	float maximumContactForce = 0.f;
	float bendMagnitude = 0.f;
	float maximumBendMagnitude = 0.f;
	std::uint32_t activeContacts = 0;
	std::uint32_t rejectedStrikes = 0;
	std::uint32_t recoveries = 0;
	bool modelRevisionMismatch = false;
};

// Reference V4: one fixed-coordinate mechanical state drives bend, wire,
// contact, cap, mount, observation, and display. The bundled coefficients are
// an estimated nominal closed-coil geometry pending specimen measurements.
class ContactHelixEngine {
public:
	static constexpr int COORDINATE_COUNT = 32;
	static constexpr int CONTACT_COUNT = 16;
	static constexpr int PULSE_CAPACITY = 4;
	static constexpr int MODEL_DATA_REVISION = 1;

	void reset();
	void resetMotion();
	void restoreFactoryFresh();
	void setSampleRate(float newSampleRate);
	void setBreakIn(float amount);
	void setBreakInLocked(bool locked) { breakInLocked = locked; }
	void setSpecimenSeed(std::uint32_t seed);
	void setRequestedModelDataRevision(int revision);
	void setLiveTuning(float excitation, float bend, float twang,
		float contact, float decay, float output);
	void setAdvancedTuning(float pitch, float dispersion, float sweep,
		float metal, float contactLoss, float gap, float capMass,
		float mountCompliance, float bendDecay);
	void setMotionTuning(float bendRate, float transfer, float radiation);
	void strike(float normalizedVelocity);
	Frame process(float requestedSampleTime);

	bool isSleeping() const { return sleeping; }
	float getBreakIn() const { return breakIn; }
	bool isBreakInLocked() const { return breakInLocked; }
	std::uint32_t getSpecimenSeed() const { return specimenSeed; }
	float getVisualMaximumDisplacement() const { return 2.75f; }
	int getModelDataRevision() const { return MODEL_DATA_REVISION; }
	int getRequestedModelDataRevision() const { return requestedModelDataRevision; }
	const ContactHelixDiagnostics& getDiagnostics() const { return diagnostics; }

private:
	struct Pulse { float amplitude = 0.f; int remaining = 0; int total = 0; };
	float sampleRate = 44100.f;
	float breakIn = 0.f;
	bool breakInLocked = false;
	std::uint32_t specimenSeed = 1u;
	int requestedModelDataRevision = MODEL_DATA_REVISION;
	std::array<float, COORDINATE_COUNT> q {};
	std::array<float, COORDINATE_COUNT> v {};
	std::array<float, COORDINATE_COUNT> omega {};
	std::array<float, COORDINATE_COUNT> damping {};
	std::array<float, COORDINATE_COUNT> drive {};
	std::array<float, COORDINATE_COUNT> observe {};
	std::array<Pulse, PULSE_CAPACITY> pulses {};
	float capPosition = 0.f;
	float capVelocity = 0.f;
	float mountPosition = 0.f;
	float mountVelocity = 0.f;
	float dcInput = 0.f;
	float dcOutput = 0.f;
	float strikeLight = 0.f;
	float quietTime = 0.f;
	float excitationScale = contact_helix_defaults::EXCITATION;
	float bendScale = contact_helix_defaults::BEND;
	float fundamentalScale = contact_helix_defaults::FUNDAMENTAL;
	float contactScale = contact_helix_defaults::CONTACT;
	float decayScale = contact_helix_defaults::BODY_DECAY;
	float outputScale = contact_helix_defaults::OUTPUT;
	float pitchScale = contact_helix_defaults::PITCH;
	float dispersionScale = contact_helix_defaults::DISPERSION;
	float sweepScale = contact_helix_defaults::SWEEP;
	float metalScale = contact_helix_defaults::METAL;
	float contactLossScale = contact_helix_defaults::CONTACT_LOSS;
	float gapControl = contact_helix_defaults::GAP;
	float capMassScale = contact_helix_defaults::CAP_MASS;
	float mountComplianceScale = contact_helix_defaults::MOUNT_COMPLIANCE;
	float bendDecayScale = contact_helix_defaults::BEND_DECAY;
	float bendRateScale = contact_helix_defaults::BEND_RATE;
	float transferScale = contact_helix_defaults::TRANSFER;
	float radiationControl = contact_helix_defaults::RADIATION;
	bool sleeping = true;
	ContactHelixDiagnostics diagnostics {};

	void updateCoefficients();
	float processSubstep(float h);
	float contactGap(int contact) const;
	float contactJacobian(int contact, int coordinate) const;
	float calculateEnergy() const;
	float effectiveOmegaSquared(int coordinate) const;
	bool finiteState() const;
};

} // namespace doorstop
