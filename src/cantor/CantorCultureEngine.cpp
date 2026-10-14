#include "CantorCultureEngine.hpp"

#include <algorithm>
#include <cmath>

namespace cantor {
namespace {

constexpr float kLog2Three = 1.584962500721156f;
constexpr float kLog2Five = 2.321928094887362f;
constexpr float kLog2Seven = 2.807354922057604f;
constexpr float kLog2Eleven = 3.459431618637297f;
constexpr int kTemporaryCandidateCapacity = 1024;

struct TemporaryCandidate {
	float pitchClass = 0.f;
	float complexity = 0.f;
};

float finiteOr(float value, float fallback) {
	return std::isfinite(value) ? value : fallback;
}

} // namespace

CultureEngine::CultureEngine() {
	buildCandidateTable();
	buildHarmonicCosts();
	reset();
}

float CultureEngine::clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

float CultureEngine::circularDistance(float a, float b) {
	float difference = std::fabs(a - b);
	difference -= std::floor(difference);
	return std::min(difference, 1.f - difference);
}

void CultureEngine::buildCandidateTable() {
	std::array<TemporaryCandidate, kTemporaryCandidateCapacity> temporary {};
	int temporaryCount = 0;
	constexpr float duplicateTolerance = 0.75f / 1200.f;

	for (int exponent3 = -4; exponent3 <= 4; ++exponent3) {
		for (int exponent5 = -3; exponent5 <= 3; ++exponent5) {
			for (int exponent7 = -2; exponent7 <= 2; ++exponent7) {
				for (int exponent11 = -1; exponent11 <= 1; ++exponent11) {
					const float rawPitch =
						float(exponent3) * kLog2Three
						+ float(exponent5) * kLog2Five
						+ float(exponent7) * kLog2Seven
						+ float(exponent11) * kLog2Eleven;
					float pitchClass = rawPitch - std::floor(rawPitch);
					if (pitchClass >= 1.f - duplicateTolerance) {
						pitchClass = 0.f;
					}
					const float complexity =
						float(std::abs(exponent3))
						+ 1.35f * float(std::abs(exponent5))
						+ 1.75f * float(std::abs(exponent7))
						+ 2.25f * float(std::abs(exponent11));

					int duplicate = -1;
					for (int i = 0; i < temporaryCount; ++i) {
						if (circularDistance(
							temporary[size_t(i)].pitchClass, pitchClass)
							<= duplicateTolerance) {
							duplicate = i;
							break;
						}
					}
					if (duplicate >= 0) {
						if (complexity < temporary[size_t(duplicate)].complexity) {
							temporary[size_t(duplicate)].pitchClass = pitchClass;
							temporary[size_t(duplicate)].complexity = complexity;
						}
					}
					else if (temporaryCount < kTemporaryCandidateCapacity) {
						TemporaryCandidate& inserted =
							temporary[size_t(temporaryCount++)];
						inserted.pitchClass = pitchClass;
						inserted.complexity = complexity;
					}
				}
			}
		}
	}

	std::sort(
		temporary.begin(), temporary.begin() + temporaryCount,
		[](const TemporaryCandidate& a, const TemporaryCandidate& b) {
			if (a.complexity != b.complexity) return a.complexity < b.complexity;
			return a.pitchClass < b.pitchClass;
		});
	candidateCount = std::min(temporaryCount, kMaximumCandidates);
	for (int i = 0; i < candidateCount; ++i) {
		candidates[size_t(i)].pitchClass = temporary[size_t(i)].pitchClass;
		candidates[size_t(i)].complexity = temporary[size_t(i)].complexity;
	}
	std::sort(
		candidates.begin(), candidates.begin() + candidateCount,
		[](const Candidate& a, const Candidate& b) {
			return a.pitchClass < b.pitchClass;
		});
	unisonIndex = 0;
	float closestToUnison = 1.f;
	for (int i = 0; i < candidateCount; ++i) {
		const float distance = circularDistance(candidates[size_t(i)].pitchClass, 0.f);
		if (distance < closestToUnison) {
			closestToUnison = distance;
			unisonIndex = i;
		}
	}
}

void CultureEngine::buildHarmonicCosts() {
	for (int from = 0; from < candidateCount; ++from) {
		for (int to = 0; to < candidateCount; ++to) {
			float interval = candidates[size_t(to)].pitchClass
				- candidates[size_t(from)].pitchClass;
			interval -= std::floor(interval);
			float best = 1000.f;
			for (int relation = 0; relation < candidateCount; ++relation) {
				const Candidate& rational = candidates[size_t(relation)];
				const float errorCents =
					1200.f * circularDistance(interval, rational.pitchClass);
				const float cost = 0.115f * rational.complexity
					+ 0.045f * errorCents;
				best = std::min(best, cost);
			}
			harmonicCosts[size_t(from)][size_t(to)] = best;
		}
	}
}

void CultureEngine::reset() {
	for (VoiceState& voice : voices) {
		voice = {};
		voice.candidateIndex = unisonIndex;
	}
	hasLastSelectedPitch = false;
	lastSelectedPitch = 0.f;
	randomState = seed != 0u ? seed : 1u;
}

void CultureEngine::setSeed(std::uint32_t newSeed) {
	seed = newSeed != 0u ? newSeed : 1u;
	randomState = seed;
}

std::uint32_t CultureEngine::getSeed() const {
	return seed;
}

std::uint32_t CultureEngine::getRandomState() const {
	return randomState;
}

void CultureEngine::setRandomState(std::uint32_t state) {
	randomState = state != 0u ? state : seed;
	if (randomState == 0u) randomState = 1u;
}

float CultureEngine::nextRandom01() {
	randomState ^= randomState << 13;
	randomState ^= randomState >> 17;
	randomState ^= randomState << 5;
	if (randomState == 0u) randomState = 1u;
	return float(randomState & 0x00ffffffu) * (1.f / 16777216.f);
}

CultureDecision CultureEngine::noteOn(
	int requestedVoice,
	float requestedPitch,
	const CultureSettings& requestedSettings) {
	const int voiceIndex = std::max(0, std::min(requestedVoice, kMaximumVoices - 1));
	VoiceState& requestingVoice = voices[size_t(voiceIndex)];
	requestingVoice.active = false;

	CultureSettings settings;
	settings.intent = clamp01(finiteOr(requestedSettings.intent, 0.72f));
	settings.coherence = clamp01(finiteOr(requestedSettings.coherence, 0.78f));
	settings.interpret = clamp01(finiteOr(requestedSettings.interpret, 0.f));
	settings.field = clamp01(finiteOr(requestedSettings.field, 0.42f));
	requestedPitch = std::max(-10.f, std::min(finiteOr(requestedPitch, 0.f), 10.f));

	const float complexityLimit = 1.6f + 9.2f * settings.field;
	const float intentRadius = 0.30f - 0.20f * settings.intent;
	const float distanceWeight = 0.9f + 6.5f * settings.intent;
	const float coherenceWeight = 5.2f * settings.coherence;

	std::array<float, kMaximumCandidates> pitches {};
	std::array<float, kMaximumCandidates> scores {};
	std::array<float, kMaximumCandidates> harmonicParts {};
	std::array<int, kMaximumCandidates> indices {};
	int consideredCount = 0;
	float bestScore = 1e9f;
	int bestConsidered = 0;
	int activeContextVoices = 0;
	for (const VoiceState& voice : voices) {
		if (voice.active) ++activeContextVoices;
	}

	for (int candidateIndex = 0;
		candidateIndex < candidateCount && consideredCount < kMaximumCandidates;
		++candidateIndex) {
		const Candidate& candidate = candidates[size_t(candidateIndex)];
		if (candidate.complexity > complexityLimit) continue;
		const float octave = std::floor(
			requestedPitch - candidate.pitchClass + 0.5f);
		const float pitch = octave + candidate.pitchClass;
		const float distance = std::fabs(pitch - requestedPitch);
		if (distance > intentRadius) continue;

		float harmonicCost = 0.f;
		if (activeContextVoices > 0) {
			for (const VoiceState& voice : voices) {
				if (!voice.active) continue;
				harmonicCost += harmonicCosts[size_t(voice.candidateIndex)]
					[size_t(candidateIndex)];
			}
			harmonicCost /= float(activeContextVoices);
		}
		else {
			harmonicCost = harmonicCosts[size_t(unisonIndex)][size_t(candidateIndex)];
		}

		float score = distanceWeight * (distance / std::max(intentRadius, 1e-4f))
			+ coherenceWeight * harmonicCost
			+ 0.055f * candidate.complexity;
		if (hasLastSelectedPitch
			&& std::fabs(pitch - lastSelectedPitch) < (1.f / 1200.f)) {
			score += 0.55f * settings.interpret;
		}
		pitches[size_t(consideredCount)] = pitch;
		scores[size_t(consideredCount)] = score;
		harmonicParts[size_t(consideredCount)] = harmonicCost;
		indices[size_t(consideredCount)] = candidateIndex;
		if (score < bestScore) {
			bestScore = score;
			bestConsidered = consideredCount;
		}
		++consideredCount;
	}

	if (consideredCount == 0) {
		pitches[0] = requestedPitch;
		scores[0] = 0.f;
		harmonicParts[0] = 0.f;
		indices[0] = unisonIndex;
		consideredCount = 1;
		bestConsidered = 0;
		bestScore = 0.f;
	}

	int selected = bestConsidered;
	if (settings.interpret > 1e-4f && consideredCount > 1) {
		const float temperature = 0.035f + 0.42f * settings.interpret;
		std::array<float, kMaximumCandidates> weights {};
		float totalWeight = 0.f;
		for (int i = 0; i < consideredCount; ++i) {
			const float relativeScore = scores[size_t(i)] - bestScore;
			const float weight = relativeScore < 8.f * temperature
				? std::exp(-relativeScore / temperature) : 0.f;
			weights[size_t(i)] = weight;
			totalWeight += weight;
		}
		float target = nextRandom01() * totalWeight;
		for (int i = 0; i < consideredCount; ++i) {
			target -= weights[size_t(i)];
			if (target <= 0.f) {
				selected = i;
				break;
			}
		}
	}

	const int selectedCandidate = indices[size_t(selected)];
	const float selectedPitch = std::max(
		-10.f, std::min(pitches[size_t(selected)], 10.f));
	requestingVoice.heldPitch = selectedPitch;
	requestingVoice.candidateIndex = selectedCandidate;
	requestingVoice.active = true;
	lastSelectedPitch = selectedPitch;
	hasLastSelectedPitch = true;

	CultureDecision decision;
	decision.requestedPitch = requestedPitch;
	decision.selectedPitch = selectedPitch;
	decision.selectedScore = scores[size_t(selected)];
	decision.distanceCents = 1200.f * std::fabs(selectedPitch - requestedPitch);
	decision.harmonicCost = harmonicParts[size_t(selected)];
	decision.complexity = candidates[size_t(selectedCandidate)].complexity;
	decision.candidateIndex = selectedCandidate;
	decision.activeContextVoices = activeContextVoices;
	return decision;
}

CultureDecision CultureEngine::quantizeStatic(
	float requestedPitch,
	const CultureSettings& requestedSettings) {
	const auto savedVoices = voices;
	const float savedLastSelectedPitch = lastSelectedPitch;
	const bool savedHasLastSelectedPitch = hasLastSelectedPitch;
	const std::uint32_t savedRandomState = randomState;
	for (VoiceState& voice : voices) {
		voice.active = false;
	}
	CultureSettings settings = requestedSettings;
	settings.interpret = 0.f;
	const CultureDecision decision = noteOn(0, requestedPitch, settings);
	voices = savedVoices;
	lastSelectedPitch = savedLastSelectedPitch;
	hasLastSelectedPitch = savedHasLastSelectedPitch;
	randomState = savedRandomState;
	return decision;
}

void CultureEngine::noteOff(int voice) {
	if (voice < 0 || voice >= kMaximumVoices) return;
	voices[size_t(voice)].active = false;
}

bool CultureEngine::isVoiceActive(int voice) const {
	return voice >= 0 && voice < kMaximumVoices
		? voices[size_t(voice)].active : false;
}

float CultureEngine::getHeldPitch(int voice) const {
	return voice >= 0 && voice < kMaximumVoices
		? voices[size_t(voice)].heldPitch : 0.f;
}

int CultureEngine::getActiveVoiceCount() const {
	int count = 0;
	for (const VoiceState& voice : voices) {
		if (voice.active) ++count;
	}
	return count;
}

int CultureEngine::getCandidateCount() const {
	return candidateCount;
}

} // namespace cantor
