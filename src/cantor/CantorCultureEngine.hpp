#pragma once

#include <array>
#include <cstdint>

namespace cantor {

constexpr int kMaximumVoices = 16;
constexpr int kMaximumCandidates = 192;

struct CultureSettings {
	float intent = 0.72f;
	float coherence = 0.78f;
	float interpret = 0.f;
	float field = 0.42f;
};

struct CultureDecision {
	float requestedPitch = 0.f;
	float selectedPitch = 0.f;
	float selectedScore = 0.f;
	float distanceCents = 0.f;
	float harmonicCost = 0.f;
	float complexity = 0.f;
	int candidateIndex = 0;
	int activeContextVoices = 0;
};

class CultureEngine {
public:
	CultureEngine();

	void reset();
	void setSeed(std::uint32_t seed);
	std::uint32_t getSeed() const;
	std::uint32_t getRandomState() const;
	void setRandomState(std::uint32_t state);

	CultureDecision noteOn(
		int voice,
		float requestedPitch,
		const CultureSettings& settings);
	CultureDecision quantizeStatic(
		float requestedPitch,
		const CultureSettings& settings);
	void noteOff(int voice);

	bool isVoiceActive(int voice) const;
	float getHeldPitch(int voice) const;
	int getActiveVoiceCount() const;
	int getCandidateCount() const;

private:
	struct Candidate {
		float pitchClass = 0.f;
		float complexity = 0.f;
	};

	struct VoiceState {
		float heldPitch = 0.f;
		int candidateIndex = 0;
		bool active = false;
	};

	std::array<Candidate, kMaximumCandidates> candidates {};
	std::array<std::array<float, kMaximumCandidates>, kMaximumCandidates>
		harmonicCosts {};
	std::array<VoiceState, kMaximumVoices> voices {};
	int candidateCount = 0;
	int unisonIndex = 0;
	float lastSelectedPitch = 0.f;
	bool hasLastSelectedPitch = false;
	std::uint32_t seed = 1u;
	std::uint32_t randomState = 1u;

	void buildCandidateTable();
	void buildHarmonicCosts();
	float nextRandom01();
	static float clamp01(float value);
	static float circularDistance(float a, float b);
};

} // namespace cantor
