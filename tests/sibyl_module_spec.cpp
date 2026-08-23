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

sibyl::CompositionPtr makeTimingComposition(float swing, float microshift, int ratchets = 1, float gate = 0.5f) {
	std::shared_ptr<sibyl::Composition> composition = std::const_pointer_cast<sibyl::Composition>(
		makeComposition(1, false));
	composition->meta.bpm = 60.0f;
	composition->meta.swing = swing;
	auto& pattern = composition->patterns["first"];
	pattern.steps.clear();
	sibyl::StepEvent event;
	event.step = 1;
	event.compiledPitchV = 3.0f;
	event.microshift = microshift;
	event.ratchets = ratchets;
	event.hasGate = true;
	event.gate = gate;
	pattern.steps.push_back(event);
	pattern.eventIndexByStep.assign(pattern.length, -1);
	pattern.eventIndexByStep[1] = 0;
	return composition;
}

sibyl::CompositionPtr makeTiedSwingComposition() {
	std::shared_ptr<sibyl::Composition> composition = std::const_pointer_cast<sibyl::Composition>(
		makeComposition(1, false));
	composition->meta.bpm = 60.0f;
	composition->meta.swing = 0.2f;
	auto& pattern = composition->patterns["first"];
	pattern.steps.clear();
	sibyl::StepEvent first;
	first.step = 0;
	first.compiledPitchV = 1.0f;
	first.hasGate = true;
	first.gate = 0.2f;
	pattern.steps.push_back(first);
	sibyl::StepEvent tied;
	tied.step = 1;
	tied.compiledPitchV = 2.0f;
	tied.tie = true;
	pattern.steps.push_back(tied);
	pattern.eventIndexByStep.assign(pattern.length, -1);
	pattern.eventIndexByStep[0] = 0;
	pattern.eventIndexByStep[1] = 1;
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

	{
		SibylModule module;
		module.acceptComposition(makeTimingComposition(0.2f, 0.0f), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		Module::ProcessArgs args;
		args.sampleRate = 1000.0f;
		args.sampleTime = 0.001f;
		module.process(args);
		module.m_trackStates[0].patternPhaseBeats = 0.0;
		module.m_trackStates[0].lastFiredStep = -1;
		module.m_trackStates[0].activeEventStep = -1;
		for (int i = 0; i < 299; ++i) { args.frame = i; module.process(args); }
		check(module.m_trackStates[0].lastFiredStep != 1, "swing holds the odd event until its delayed onset");
		args.frame = 299; module.process(args);
		check(module.m_trackStates[0].lastFiredStep == 1 &&
			std::abs(module.outputs[SibylModule::V_OCT_OUTPUT].getVoltage(0) - 3.0f) < 1e-6f,
			"swing fires the odd event at the shifted sample");
	}

	{
		SibylModule module;
		module.acceptComposition(makeTimingComposition(0.0f, -0.2f, 2, 0.25f), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		Module::ProcessArgs args;
		args.sampleRate = 1000.0f;
		args.sampleTime = 0.001f;
		module.process(args);
		module.m_trackStates[0].patternPhaseBeats = 0.0;
		module.m_trackStates[0].lastFiredStep = -1;
		module.m_trackStates[0].activeEventStep = -1;
		module.m_trackStates[0].activeEventPlayed = false;
		module.m_trackStates[0].currentGate = 0.0f;
		for (int i = 0; i < 199; ++i) { args.frame = i; module.process(args); }
		check(module.m_trackStates[0].lastFiredStep != 1, "negative microshift does not fire before its authored early onset");
		args.frame = 199; module.process(args);
		check(module.m_trackStates[0].lastFiredStep == 1, "negative microshift advances the event onset");
		for (int i = 200; i < 240; ++i) { args.frame = i; module.process(args); }
		check(module.outputs[SibylModule::GATE_OUTPUT].getVoltage(0) == 0.0f,
			"ratchet gate closes within its first shifted slice");
		for (int i = 240; i < 330; ++i) { args.frame = i; module.process(args); }
		check(module.outputs[SibylModule::GATE_OUTPUT].getVoltage(0) > 9.0f,
			"ratchet reopens from the shifted event-relative clock");
	}

	{
		SibylModule module;
		module.acceptComposition(makeTiedSwingComposition(), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		Module::ProcessArgs args;
		args.sampleRate = 1000.0f;
		args.sampleTime = 0.001f;
		module.process(args);
		module.m_trackStates[0].patternPhaseBeats = 0.0;
		module.m_trackStates[0].lastFiredStep = -1;
		module.m_trackStates[0].activeEventStep = -1;
		for (int i = 0; i < 299; ++i) { args.frame = i; module.process(args); }
		check(module.outputs[SibylModule::GATE_OUTPUT].getVoltage(0) > 9.0f,
			"a delayed tied event holds the preceding gate through the swing gap");
		args.frame = 299; module.process(args);
		check(module.m_trackStates[0].lastFiredStep == 1 &&
			module.outputs[SibylModule::GATE_OUTPUT].getVoltage(0) > 9.0f,
			"tied shifted onset changes pitch without retriggering or dropping the gate");
	}

	std::cout << "[SUMMARY] sibyl_module_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
