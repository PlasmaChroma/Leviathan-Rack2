#include "SibylAdoption.hpp"

#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}

sibyl::Composition compositionWithTrack() {
	sibyl::Composition composition;
	sibyl::TrackDef track;
	track.id = "bass";
	track.channel = 3;
	composition.tracks.push_back(track);
	sibyl::Pattern pattern;
	pattern.id = "line";
	sibyl::StepEvent event;
	event.step = 0;
	pattern.steps.push_back(event);
	composition.patterns[pattern.id] = pattern;
	sibyl::Scene scene;
	scene.id = "main";
	sibyl::TrackAssignment assignment;
	assignment.patternId = pattern.id;
	scene.tracks[track.id] = assignment;
	composition.arrangement.push_back(scene);
	return composition;
}
}

int main() {
	sibyl::ApplyAt applyAt;
	check(sibyl::parseApplyAtName("nextScene", applyAt) && applyAt == sibyl::ApplyAt::NEXT_SCENE,
		"apply boundary names parse strictly");
	check(!sibyl::parseApplyAtName("later", applyAt), "unknown apply boundary is rejected");
	sibyl::PhasePolicy policy;
	check(sibyl::parsePhasePolicyName("restartChanged", policy) && policy == sibyl::PhasePolicy::RESTART_CHANGED,
		"phase policy names parse strictly");
	check(!sibyl::parsePhasePolicyName("maybe", policy), "unknown phase policy is rejected");

	sibyl::BoundaryState none;
	sibyl::BoundaryState step; step.step = true;
	sibyl::BoundaryState beat; beat.beat = true;
	sibyl::BoundaryState scene; scene.scene = true;
	check(sibyl::adoptionBoundaryReached(sibyl::ApplyAt::IMMEDIATE, none), "immediate adopts without a musical boundary");
	check(sibyl::adoptionBoundaryReached(sibyl::ApplyAt::NEXT_STEP, step) && !sibyl::adoptionBoundaryReached(sibyl::ApplyAt::NEXT_STEP, beat),
		"nextStep waits for a step boundary");
	check(sibyl::adoptionBoundaryReached(sibyl::ApplyAt::NEXT_BEAT, beat), "nextBeat waits for a beat boundary");
	check(sibyl::adoptionBoundaryReached(sibyl::ApplyAt::NEXT_SCENE, scene), "nextScene waits for a scene boundary");

	auto original = compositionWithTrack();
	auto unchanged = original;
	check(sibyl::changedTrackChannelMask(original, unchanged) == 0, "unchanged material preserves every track");
	auto eventChanged = original;
	eventChanged.patterns["line"].steps[0].gate = 0.75f;
	eventChanged.patterns["line"].steps[0].hasGate = true;
	check(sibyl::changedTrackChannelMask(original, eventChanged) == (1u << 3), "pattern edits mark only their track channel");
	auto unrelated = original;
	sibyl::TrackDef second; second.id = "lead"; second.channel = 7;
	unrelated.tracks.push_back(second);
	check(sibyl::changedTrackChannelMask(original, unrelated) == (1u << 7), "new tracks do not restart unchanged channels");

	std::cout << "[SUMMARY] sibyl_adoption_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
