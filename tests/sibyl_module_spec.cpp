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
	composition->clock.externalPpqn = 4;
	composition->clock.outputPpqn = 1;
	composition->clock.externalTimeoutMs = 1000.0f;

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

	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		module.inputs[SibylModule::CLOCK_INPUT].channels = 1;
		Module::ProcessArgs args;
		args.sampleRate = 1000.0f;
		args.sampleTime = 0.001f;
		module.inputs[SibylModule::CLOCK_INPUT].setVoltage(0.0f);
		module.process(args); // Adopt before the first external edge.
		bool previousClockOut = false;
		int outputRisingEdges = 0;
		for (int frame = 0; frame < 500; ++frame) {
			module.inputs[SibylModule::CLOCK_INPUT].setVoltage(frame % 125 == 0 ? 10.0f : 0.0f);
			args.frame = frame;
			module.process(args);
			bool clockOut = module.outputs[SibylModule::CLOCK_OUTPUT].getVoltage() > 5.0f;
			if (clockOut && !previousClockOut) ++outputRisingEdges;
			previousClockOut = clockOut;
		}
		std::cout << "[INFO] reconstructed clock rising edges: " << outputRisingEdges << "\n";
		check(outputRisingEdges == 1,
			"reconstructed CLOCK OUT honors output PPQN instead of copying every external edge");
		double estimatedBpm = module.m_externalClockEstimator.estimatedBpm(4, 0.0);
		std::cout << "[INFO] module estimated BPM: " << estimatedBpm << "\n";
		check(std::abs(estimatedBpm - 120.0) < 1.0,
			"module clock estimator converges on the external tempo");

		double learnedInterval = module.m_externalClockEstimator.intervalSeconds();
		module.m_outputClockPhaseBeats = 0.73;
		sibyl::TransportRequest restart;
		restart.action = sibyl::TransportAction::RESTART;
		restart.target = sibyl::RestartTarget::ARRANGEMENT;
		restart.applyAt = sibyl::ApplyAt::IMMEDIATE;
		sibyl::BoundaryState boundary;
		const sibyl::Composition* active = module.m_activeCompositionPtr.load(std::memory_order_acquire);
		module.applyPendingTransportIfReady(boundary, *active, &restart);
		check(module.m_outputClockPhaseBeats == 0.0,
			"arrangement restart realigns the independent CLOCK OUT phase");
		check(std::abs(module.m_externalClockEstimator.intervalSeconds() - learnedInterval) < 1e-12,
			"musical restart preserves the learned external clock estimate");

		module.m_outputClockPhaseBeats = 0.41;
		sibyl::TransportRequest randomnessRestart = restart;
		randomnessRestart.target = sibyl::RestartTarget::RANDOMNESS;
		module.applyPendingTransportIfReady(boundary, *active, &randomnessRestart);
		check(std::abs(module.m_outputClockPhaseBeats - 0.41) < 1e-12,
			"randomness-only restart does not disturb CLOCK OUT phase");
	}

	std::cout << "[SUMMARY] sibyl_module_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
