#define SIBYL_MODULE_TEST
#include "../src/Sibyl.cpp"

#include <cmath>
#include <iostream>

Plugin* pluginInstance = nullptr;

namespace {
int failures = 0;

void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}

sibyl::CompositionPtr makeComposition(int revision, bool twoScenes, float destinationPitch = 2.0f) {
	std::shared_ptr<sibyl::Composition> composition(new sibyl::Composition());
	composition->revision = revision;
	composition->meta.bpm = 120.0f;
	composition->transport.running = true;
	composition->transport.loop = true;

	sibyl::TrackDef track;
	track.id = "voice";
	track.channel = 0;
	track.defaultGate = 1.0f;
	track.defaultVelocity = 1.0f;
	composition->tracks.push_back(track);

	sibyl::Pattern firstPattern;
	firstPattern.id = "first";
	firstPattern.length = 4;
	firstPattern.resolutionStr = "1/16";
	firstPattern.resolutionBeats = 0.25;
	sibyl::StepEvent firstEvent;
	firstEvent.step = 0;
	firstEvent.compiledPitchV = 1.0f;
	firstEvent.hasGate = true;
	firstEvent.gate = 1.0f;
	firstPattern.steps.push_back(firstEvent);
	composition->patterns[firstPattern.id] = firstPattern;

	sibyl::Scene firstScene;
	firstScene.id = "first_scene";
	firstScene.lengthBeats = twoScenes ? 0.00001f : 4.0f;
	firstScene.repeats = 1;
	sibyl::TrackAssignment firstAssignment;
	firstAssignment.patternId = firstPattern.id;
	firstScene.tracks[track.id] = firstAssignment;
	composition->arrangement.push_back(firstScene);

	if (twoScenes) {
		sibyl::Pattern destinationPattern = firstPattern;
		destinationPattern.id = "destination";
		destinationPattern.steps[0].compiledPitchV = destinationPitch;
		composition->patterns[destinationPattern.id] = destinationPattern;
		sibyl::Scene destinationScene;
		destinationScene.id = "destination_scene";
		destinationScene.lengthBeats = 4.0f;
		destinationScene.repeats = 1;
		destinationScene.phaseMode = sibyl::PhaseMode::RESTART;
		sibyl::TrackAssignment destinationAssignment;
		destinationAssignment.patternId = destinationPattern.id;
		destinationScene.tracks[track.id] = destinationAssignment;
		composition->arrangement.push_back(destinationScene);
	}
	return composition;
}

void processOneSample(SibylModule& module) {
	Module::ProcessArgs args;
	args.sampleRate = 48000.0f;
	args.sampleTime = 1.0f / args.sampleRate;
	args.frame = 0;
	module.process(args);
}
}

int main() {
	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, true), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		check(module.m_sceneIndex == 1, "scene boundary enters the destination scene on the crossing sample");
		check(module.m_trackStates[0].lastFiredStep == 0,
			"destination restart generates its step-zero event on the adoption sample");
		check(std::abs(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage(0) - 2.0f) < 1e-6f,
			"destination event, not the stale source scene, drives pitch");
		check(module.outputs[SibylModule::GATE_OUTPUT].getVoltage(0) > 9.0f,
			"destination step-zero gate opens immediately");
	}

	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		module.acceptComposition(makeComposition(2, false), sibyl::ApplyAt::NEXT_BEAT,
			sibyl::PhasePolicy::PRESERVE);
		module.acceptComposition(makeComposition(3, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_CHANGED);
		processOneSample(module);
		check(module.m_activeRevision.load(std::memory_order_acquire) == 3,
			"coalesced edits adopt only the newest accepted revision");
		check(module.m_pendingAdoptionPtr.load(std::memory_order_acquire) == nullptr,
			"newest coalesced adoption clears pending state at its boundary");
	}

	std::cout << "[SUMMARY] sibyl_module_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
