#include "SibylHardwareControl.hpp"

#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
}

int main() {
	check(sibyl::kHardwareSchmittLowVolts == 0.1f && sibyl::kHardwareSchmittHighVolts == 1.0f,
		"hardware Schmitt thresholds are explicit");
	check(sibyl::sceneIndexFromCv(-2.0f, 4, -1) == 0 &&
		sibyl::sceneIndexFromCv(10.0f, 4, -1) == 3,
		"scene CV clamps to the arrangement range");
	check(sibyl::sceneIndexFromCv(2.60f, 4, 0) == 0 &&
		sibyl::sceneIndexFromCv(2.64f, 4, 0) == 1,
		"scene CV resists upward boundary noise");
	check(sibyl::sceneIndexFromCv(2.40f, 4, 1) == 1 &&
		sibyl::sceneIndexFromCv(2.36f, 4, 1) == 0,
		"scene CV resists downward boundary noise");

	sibyl::BoundaryState none;
	sibyl::BoundaryState beat;
	beat.beat = true;
	check(!sibyl::hardwareResetBoundaryReached(false, false, none) &&
		sibyl::hardwareResetBoundaryReached(false, false, beat),
		"internal reset waits for the next beat");
	check(!sibyl::hardwareResetBoundaryReached(true, false, beat) &&
		sibyl::hardwareResetBoundaryReached(true, true, none),
		"external reset waits for the next detected clock edge");
	check(sibyl::hardwareRunFallingEdge(true, false) &&
		!sibyl::hardwareRunFallingEdge(false, false) &&
		!sibyl::hardwareRunFallingEdge(false, true),
		"only a RUN falling edge requests immediate gate closure");

	sibyl::BoundaryState step;
	step.step = true;
	sibyl::BoundaryState sceneBoundary;
	sceneBoundary.scene = true;
	check(sibyl::hardwareSceneBoundaryReached(sibyl::ApplyAt::IMMEDIATE, none),
		"immediate hardware scene selection applies on the next sample");
	check(sibyl::hardwareSceneBoundaryReached(sibyl::ApplyAt::NEXT_STEP, step) &&
		!sibyl::hardwareSceneBoundaryReached(sibyl::ApplyAt::NEXT_STEP, beat),
		"hardware scene selection honors nextStep");
	check(sibyl::hardwareSceneBoundaryReached(sibyl::ApplyAt::NEXT_BEAT, beat),
		"hardware scene selection honors nextBeat");
	check(sibyl::hardwareSceneBoundaryReached(sibyl::ApplyAt::NEXT_SCENE, sceneBoundary),
		"hardware scene selection honors nextScene");

	sibyl::Scene scene;
	scene.phaseMode = sibyl::PhaseMode::CONTINUE;
	sibyl::TrackAssignment assignment;
	assignment.patternId = "line";
	assignment.hasPhaseModeOverride = true;
	assignment.phaseModeOverride = sibyl::PhaseMode::ALIGN_GLOBAL;
	scene.tracks["lead"] = assignment;
	check(sibyl::destinationTrackPhaseMode(scene, "bass") == sibyl::PhaseMode::CONTINUE,
		"scene phase mode is the destination default");
	check(sibyl::destinationTrackPhaseMode(scene, "lead") == sibyl::PhaseMode::ALIGN_GLOBAL,
		"track assignment overrides destination phase mode");

	std::cout << "[SUMMARY] sibyl_hardware_control_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
