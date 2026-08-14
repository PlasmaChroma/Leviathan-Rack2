#include "../src/CantorCultureEngine.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>

namespace {

struct Result {
	std::string name;
	bool passed = false;
	std::string detail;
};

Result boundedFieldBuilds() {
	cantor::CultureEngine engine;
	const int count = engine.getCandidateCount();
	return {
		"Bounded 11-limit field is precomputed",
		count >= 96 && count <= cantor::kMaximumCandidates,
		"candidates=" + std::to_string(count)
	};
}

Result exactRationalIntentSurvives() {
	cantor::CultureEngine engine;
	cantor::CultureSettings settings;
	settings.intent = 1.f;
	settings.coherence = 0.f;
	settings.field = 1.f;
	const float justMajorThird = std::log2(5.f / 4.f);
	const auto decision = engine.noteOn(0, justMajorThird, settings);
	const float errorCents =
		1200.f * std::fabs(decision.selectedPitch - justMajorThird);
	return {
		"A requested exact 5:4 survives literal Culture interpretation",
		errorCents < 0.1f,
		"errorCents=" + std::to_string(errorCents)
	};
}

Result existingVoicesRemainSovereign() {
	cantor::CultureEngine engine;
	cantor::CultureSettings settings;
	const float first = engine.noteOn(0, 0.22f, settings).selectedPitch;
	engine.noteOn(1, 0.68f, settings);
	const float after = engine.getHeldPitch(0);
	const bool activeBeforeRelease = engine.getActiveVoiceCount() == 2;
	engine.noteOff(0);
	return {
		"Existing notes stay fixed and gate-off only removes harmonic context",
		std::fabs(first - after) < 1e-7f
			&& activeBeforeRelease
			&& !engine.isVoiceActive(0)
			&& engine.isVoiceActive(1)
			&& std::fabs(engine.getHeldPitch(0) - first) < 1e-7f,
		"held=" + std::to_string(first) + "/" + std::to_string(after)
			+ " active=" + std::to_string(engine.getActiveVoiceCount())
	};
}

Result coherenceChangesNegotiation() {
	int changedSelections = 0;
	int improvedRelationships = 0;
	float totalLiteralCost = 0.f;
	float totalCoherentCost = 0.f;
	for (int step = 0; step <= 24; ++step) {
		const float request = -0.1f + float(step) * (1.2f / 24.f);
		cantor::CultureSettings anchorSettings;
		anchorSettings.intent = 1.f;
		anchorSettings.coherence = 0.f;
		anchorSettings.field = 1.f;
		cantor::CultureSettings literal = anchorSettings;
		literal.intent = 0.20f;
		cantor::CultureSettings coherent = literal;
		coherent.coherence = 1.f;

		cantor::CultureEngine literalEngine;
		cantor::CultureEngine coherentEngine;
		literalEngine.noteOn(0, std::log2(5.f / 4.f), anchorSettings);
		coherentEngine.noteOn(0, std::log2(5.f / 4.f), anchorSettings);
		const auto literalDecision = literalEngine.noteOn(1, request, literal);
		const auto coherentDecision = coherentEngine.noteOn(1, request, coherent);
		if (std::fabs(literalDecision.selectedPitch
			- coherentDecision.selectedPitch) > (0.5f / 1200.f)) {
			++changedSelections;
		}
		if (coherentDecision.harmonicCost
			<= literalDecision.harmonicCost + 1e-6f) {
			++improvedRelationships;
		}
		totalLiteralCost += literalDecision.harmonicCost;
		totalCoherentCost += coherentDecision.harmonicCost;
	}
	return {
		"Coherence lets new notes negotiate toward simpler active relationships",
		changedSelections >= 3 && improvedRelationships >= 23
			&& totalCoherentCost < totalLiteralCost,
		"changed=" + std::to_string(changedSelections)
			+ " improved=" + std::to_string(improvedRelationships)
			+ " costs=" + std::to_string(totalLiteralCost)
			+ "/" + std::to_string(totalCoherentCost)
	};
}

Result deterministicAndInterpretiveModesSeparate() {
	cantor::CultureSettings deterministic;
	deterministic.intent = 0.35f;
	deterministic.coherence = 0.8f;
	deterministic.field = 0.75f;
	deterministic.interpret = 0.f;
	cantor::CultureEngine first;
	cantor::CultureEngine second;
	first.setSeed(7331u);
	second.setSeed(7331u);
	bool identical = true;
	for (int i = 0; i < 24; ++i) {
		const int voice = i % 4;
		const float request = 0.37f + 0.03f * float(i % 3);
		const float a = first.noteOn(voice, request, deterministic).selectedPitch;
		const float b = second.noteOn(voice, request, deterministic).selectedPitch;
		identical = identical && std::fabs(a - b) < 1e-7f;
		first.noteOff(voice);
		second.noteOff(voice);
	}

	cantor::CultureSettings interpretive = deterministic;
	interpretive.interpret = 1.f;
	cantor::CultureEngine varying;
	varying.setSeed(91u);
	std::set<int> selectedCents;
	float maximumDistance = 0.f;
	for (int i = 0; i < 96; ++i) {
		const auto decision = varying.noteOn(0, 0.40f, interpretive);
		selectedCents.insert(int(std::lround(decision.selectedPitch * 1200.f)));
		maximumDistance = std::max(maximumDistance, decision.distanceCents);
		varying.noteOff(0);
	}
	return {
		"INTERPRET adds seeded alternatives without becoming random voltage",
		identical && selectedCents.size() >= 2 && maximumDistance <= 300.f,
		"identical=" + std::to_string(identical)
			+ " choices=" + std::to_string(selectedCents.size())
			+ " maxDistance=" + std::to_string(maximumDistance)
	};
}

Result abusiveInputsStayFiniteAndBounded() {
	cantor::CultureEngine engine;
	cantor::CultureSettings settings;
	const float requests[] = {
		-1000.f, 1000.f, INFINITY, -INFINITY, NAN, -9.9f, 9.9f
	};
	bool valid = true;
	for (int pass = 0; pass < 32; ++pass) {
		for (int voice = 0; voice < cantor::kMaximumVoices; ++voice) {
			const float request = requests[(pass + voice) % 7];
			const auto decision = engine.noteOn(voice, request, settings);
			valid = valid && std::isfinite(decision.selectedPitch)
				&& decision.selectedPitch >= -10.f
				&& decision.selectedPitch <= 10.f;
		}
	}
	return {
		"Culture selection contains non-finite and out-of-range requests",
		valid && engine.getActiveVoiceCount() == cantor::kMaximumVoices,
		"active=" + std::to_string(engine.getActiveVoiceCount())
	};
}

} // namespace

int main() {
	const Result results[] = {
		boundedFieldBuilds(),
		exactRationalIntentSurvives(),
		existingVoicesRemainSovereign(),
		coherenceChangesNegotiation(),
		deterministicAndInterpretiveModesSeparate(),
		abusiveInputsStayFiniteAndBounded(),
	};
	int failures = 0;
	for (const Result& result : results) {
		std::cout << (result.passed ? "[PASS] " : "[FAIL] ")
			<< result.name << " :: " << result.detail << '\n';
		if (!result.passed) ++failures;
	}
	std::cout << "[SUMMARY] cantor_culture_engine_spec: "
		<< (int(sizeof(results) / sizeof(results[0])) - failures)
		<< "/" << int(sizeof(results) / sizeof(results[0])) << " passed\n";
	return failures == 0 ? 0 : 1;
}
