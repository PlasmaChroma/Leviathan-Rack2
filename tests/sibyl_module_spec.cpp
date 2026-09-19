#define SIBYL_MODULE_TEST
#include "../src/Sibyl.cpp"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <new>
#include <thread>

#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
#endif

namespace {
bool gTrackAllocations = false;
std::size_t gAllocationCount = 0;
std::size_t gDeallocationCount = 0;
}

void* operator new(std::size_t size) {
	if (gTrackAllocations) ++gAllocationCount;
	if (void* memory = std::malloc(size)) return memory;
	throw std::bad_alloc();
}

void* operator new[](std::size_t size) {
	return ::operator new(size);
}

void operator delete(void* memory) noexcept { if (gTrackAllocations && memory) ++gDeallocationCount; std::free(memory); }
void operator delete[](void* memory) noexcept { if (gTrackAllocations && memory) ++gDeallocationCount; std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { if (gTrackAllocations && memory) ++gDeallocationCount; std::free(memory); }
void operator delete[](void* memory, std::size_t) noexcept { if (gTrackAllocations && memory) ++gDeallocationCount; std::free(memory); }

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
	firstEvent.hasMod = true;
	firstEvent.mod = -10.0f;
	firstEvent.hasMod2 = true;
	firstEvent.mod2 = 6.5f;
	firstEvent.hasMod3 = true;
	firstEvent.mod3 = 10.0f;
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

#include "sibyl_condition_cases.hpp"
#include "sibyl_override_cases.hpp"
#include "sibyl_automation_cases.hpp"
#include "sibyl_harmony_cases.hpp"
#include "sibyl_voicing_cases.hpp"
#include "sibyl_combined_cases.hpp"
#include "sibyl_tuning_module_cases.hpp"
#include "sibyl_native_module_cases.hpp"

int main() {
    testTunedPlayback();
    testNativeHarmonyPlayback();
    testConditions();
    testOverrides();
    testAutomation();
    testHarmony();
    testVoicing();
    testCombinedComposition();
    {
        auto composition=std::const_pointer_cast<sibyl::Composition>(makeComposition(1,false));
        composition->tracks[0].channel=6;
        SibylModule module;
        module.acceptComposition(composition,sibyl::ApplyAt::IMMEDIATE,sibyl::PhasePolicy::RESTART_ALL);
        processOneSample(module);
        bool channelsCorrect=true;
        gAllocationCount=gDeallocationCount=0;
        for(int port:{SibylModule::V_OCT_OUTPUT,SibylModule::GATE_OUTPUT,SibylModule::VELOCITY_OUTPUT,
                      SibylModule::MOD_OUTPUT,SibylModule::MOD_2_OUTPUT,SibylModule::MOD_3_OUTPUT}) {
            // Simulate Rack Engine's connection state and callback; setChannels()
            // intentionally does nothing on an unconnected SDK Output.
            module.outputs[port].channels=1;
            rack::engine::Module::PortChangeEvent event{true,rack::engine::Port::OUTPUT,port};
            module.onPortChange(event);
            gTrackAllocations=true;processOneSample(module);gTrackAllocations=false;
            channelsCorrect &= module.outputs[port].getChannels()==7;
            module.outputs[port].channels=0;event.connecting=false;module.onPortChange(event);
            processOneSample(module);
            module.outputs[port].channels=1;event.connecting=true;module.onPortChange(event);
            gTrackAllocations=true;processOneSample(module);gTrackAllocations=false;
            channelsCorrect &= module.outputs[port].getChannels()==7;
        }
        check(channelsCorrect,"live regression: late connection/reconnection restores all six polyphonic output widths");
        check(gAllocationCount==0&&gDeallocationCount==0,"live regression: output reconnection uses no audio allocations or frees");
    }

    {
        auto parsed = sibyl::parseCompositionJson(R"({"tracks":[{"id":"v","channel":0}],"patterns":{"p":{"steps":[{"step":0,"note":"C3","gate":4}]}},"arrangement":[{"id":"s","tracks":{"v":"p"}}]})", 1);
        check(parsed.valid, "P1 fixture compiles");
        SibylModule module;
        module.acceptComposition(parsed.composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
        processOneSample(module);
        json_t* undoState = module.dataToJson();
        std::string response, error;
        const std::string operations = R"([{"op":"insert_notes","pattern_id":"p","notes":[{"step":4,"note":"E3"}]},{"op":"transpose_notes","pattern_id":"p","selector":{"ids":["n2"]},"semitones":12}])";
        std::string preview = "{\"expected_revision\":1,\"operations\":" + operations + "}";
        auto* accepted = module.m_acceptedCompositionPtr;
        auto* pending = module.m_pendingAdoptionPtr.load();
        check(module.handleSibylRequest(SibylControl::Operation::VALIDATE, preview, response, error)
            && response.find("\"valid\":true") != std::string::npos
            && response.find("n2") != std::string::npos
            && module.m_acceptedCompositionPtr == accepted && module.m_pendingAdoptionPtr.load() == pending
            && module.m_acceptedRevision == 1 && accepted->patterns.at("p").nextNoteId == 2,
            "operation preview consumes no revision, ID, or pending adoption");
        json_error_t e;
        json_t* previewResult = json_loads(response.c_str(), 0, &e);
        std::string commit = "{\"expected_revision\":1,\"apply_at\":\"nextScene\",\"operations\":" + operations + "}";
        check(module.handleSibylRequest(SibylControl::Operation::EDIT, commit, response, error), "targeted transaction accepted");
        json_t* editResult = json_loads(response.c_str(), 0, &e);
        check(json_equal(json_object_get(previewResult,"changes"), json_object_get(editResult,"changes"))
            && module.m_acceptedRevision == 2 && module.m_activeRevision.load() == 1,
            "preview and commit have identical IDs/reports while accepted edit remains pending");
        json_decref(previewResult); json_decref(editResult);
        pending = module.m_pendingAdoptionPtr.load();
        check(!module.handleSibylRequest(SibylControl::Operation::VALIDATE, preview, response, error)
            && response.find("revision_conflict") != std::string::npos && module.m_pendingAdoptionPtr.load() == pending,
            "stale preview is rejected against accepted rather than active revision");
        check(module.handleSibylRequest(SibylControl::Operation::EDIT,
            R"({"expected_revision":2,"operations":[{"op":"update_notes","pattern_id":"p","selector":{"ids":["n2"]},"set":{"velocity":0.8}}]})", response, error)
            && module.m_acceptedCompositionPtr->patterns.at("p").steps.size() == 2,
            "new edit includes earlier accepted-but-pending note insertion");
        accepted = module.m_acceptedCompositionPtr; pending = module.m_pendingAdoptionPtr.load();
        check(module.handleSibylRequest(SibylControl::Operation::VALIDATE,
            R"({"expected_revision":3,"operations":[{"op":"insert_notes","pattern_id":"p","notes":[{"step":0,"note":"G3"}]}]})", response, error)
            && response.find("\"valid\":false") != std::string::npos && response.find("step_collision") != std::string::npos
            && module.m_acceptedCompositionPtr == accepted && module.m_pendingAdoptionPtr.load() == pending,
            "failed candidate preview returns valid:false without mutation");
        check(!module.handleSibylRequest(SibylControl::Operation::VALIDATE,
            R"({"expected_revision":3,"operations":[],"candidate":{}})", response, error), "mixed preview modes are malformed");

        // P8E: Two-phase commit with prepared transaction
        std::string prepPreview = R"({"expected_revision":3,"prepare":true,"operations":[{"op":"insert_notes","pattern_id":"p","notes":[{"step":8,"note":"A3"}]}]})";
        check(module.handleSibylRequest(SibylControl::Operation::VALIDATE, prepPreview, response, error)
            && response.find(R"___("valid":true)___") != std::string::npos
            && response.find(R"___("handle":)___") != std::string::npos
            && response.find(R"___("expiresInSeconds":60)___") != std::string::npos
            && module.m_acceptedRevision == 3,
            "validate with prepare:true returns opaque handle without mutating state");
        json_error_t pErr;
        json_t* prepObj = json_loads(response.c_str(), 0, &pErr);
        std::string handleStr = json_string_value(json_object_get(prepObj, "handle"));
        json_decref(prepObj);

        // Commit via handle
        std::string handleEdit = "{\"handle\":\"" + handleStr + "\"}";
        check(module.handleSibylRequest(SibylControl::Operation::EDIT, handleEdit, response, error)
            && response.find(R"___("ok":true)___") != std::string::npos
            && module.m_acceptedRevision == 4
            && module.m_acceptedCompositionPtr->patterns.at("p").steps.size() == 3,
            "edit with prepared handle atomically commits candidate to revision 4");
        std::string firstCommitResp = response;

        // Idempotent replay with same handle
        check(module.handleSibylRequest(SibylControl::Operation::EDIT, handleEdit, response, error)
            && response == firstCommitResp
            && module.m_acceptedRevision == 4,
            "retrying committed handle returns identical receipt idempotently");

        // Non-existent or invalid handle
        check(!module.handleSibylRequest(SibylControl::Operation::EDIT, R"({"handle":"prep_nonexistent"})", response, error)
            && response.find("handle_not_found") != std::string::npos,
            "unknown prepared handle is rejected with handle_not_found");
        check(module.handleSibylRequest(SibylControl::Operation::GET_COMPOSITION,
            R"({"view":"notes","pattern_id":"p","selector":{"ids":["n2"]},"fields":["id","note","effectivePitchV"]})", response, error)
            && response.find("derived") != std::string::npos && response.find("n2") != std::string::npos,
            "notes view returns selected authored and separately derived fields");
        json_t* redoState = module.dataToJson();
        module.dataFromJson(undoState);
        check(module.m_acceptedCompositionPtr->patterns.at("p").steps.size() == 1, "undo state restores original notes and allocator");
        module.dataFromJson(redoState);
        check(module.m_acceptedCompositionPtr->patterns.at("p").steps[1].id == "n2"
            && module.m_acceptedCompositionPtr->patterns.at("p").steps[1].transposeSemitones == 12,
            "redo state restores IDs and transpose through canonical patch codec");
        accepted = module.m_acceptedCompositionPtr;
        json_object_set_new(redoState,"schemaVersion",json_integer(99));
        json_object_set_new(redoState,"loopOverride",json_integer(0));
        const int loopBefore = module.m_loopOverride.load();
        module.dataFromJson(redoState);
        check(module.m_acceptedCompositionPtr == accepted && module.m_loopOverride.load() == loopBefore,
            "future patch schema cannot partially change composition or runtime preferences");
        json_decref(undoState); json_decref(redoState);
        gAllocationCount = gDeallocationCount = 0;
        gTrackAllocations = true;
        for (int frame = 0; frame < 100000; ++frame) processOneSample(module);
        gTrackAllocations = false;
        check(gAllocationCount == 0 && gDeallocationCount == 0,
            "P1 adoption and repeated playback allocate and free no audio-thread objects");
    }
    for (int mode = 0; mode < 3; ++mode) {
        auto composition = std::const_pointer_cast<sibyl::Composition>(makeComposition(1, false));
        auto& pattern = composition->patterns["first"];
        pattern.evolution.probability = mode == 0 ? 0.f : .2f;
        pattern.steps[0].hasProbability = true;
        pattern.steps[0].probability = .5f;
        pattern.steps[0].evolve = mode != 2;
        composition->arrangement[0].lengthBeats = 1.f;
        SibylModule module;
        module.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
        std::array<bool, 32> played {};
        bool sawPlay = false, sawSkip = false;
        for (int replay = 0; replay < 2; ++replay) {
            if (replay) {
                std::string response, error;
                check(module.handleSibylRequest(SibylControl::Operation::TRANSPORT,
                    R"({"action":"restart","target":"arrangement","apply_at":"immediate"})", response, error),
                    "probability replay reset accepted");
            }
            processOneSample(module);
            for (size_t pass = 0; pass < played.size(); ++pass) {
                for (int sample = 0; sample < 30000 && module.m_trackStates[0].evolution.pass < pass; ++sample)
                    processOneSample(module);
                if (module.m_trackStates[0].evolution.pass != pass) {
                    check(false, "probability traversal reaches the next repeat");
                    break;
                }
                bool value = module.m_trackStates[0].activeEventPlayed;
                if (replay) check(value == played[pass], "probability decisions replay after reset");
                else played[pass] = value;
                sawPlay |= value; sawSkip |= !value;
                if (mode != 1) check(value == played[0], "disabled and protected probability retain legacy draws");
            }
        }
        if (mode == 1) check(sawPlay && sawSkip, "probability evolution produces sounding and skipped repeats");
        std::string response, error;
        module.handleSibylRequest(SibylControl::Operation::CAPABILITIES, "{}", response, error);
        check(response.find("probability") != std::string::npos, "capabilities advertise probability evolution");
    }

	{
		auto composition = std::const_pointer_cast<sibyl::Composition>(makeComposition(1, false));
		auto& pattern = composition->patterns["first"];
		pattern.evolution.velocity = .2f;
		pattern.evolution.gate = .2f;
		pattern.evolution.glideMs = 40.f;
		pattern.evolution.mod[0] = .5f;
		pattern.evolution.mod[1] = .5f;
		pattern.evolution.mod[2] = .5f;
		pattern.steps[0].mod = 0.f;
		pattern.steps[0].mod2 = 0.f;
		pattern.steps[0].mod3 = 0.f;
		composition->tracks[0].defaultVelocity = .6f;
		composition->arrangement[0].lengthBeats = 1.f;
		composition->arrangement[0].repeats = 2;
		SibylModule module;
		module.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		check(module.m_trackStates[0].evolution.pass == 0 && module.m_trackStates[0].currentVel == 6.f
			&& module.m_trackStates[0].currentMod[0] == 0.f, "evolution first traversal is exactly authored");
		gAllocationCount = 0;
		gTrackAllocations = true;
		for (int i = 0; i < 25000; ++i) processOneSample(module);
		gTrackAllocations = false;
		const auto second = module.m_trackStates[0];
		check(gAllocationCount == 0 && second.evolution.pass == 1 && second.currentVel != 6.f
			&& second.currentMod[0] != 0.f && second.targetPitch == 1.f,
			"scene repeat evolves expression without pitch changes or audio allocations");
		for (int i = 0; i < 25000; ++i) processOneSample(module);
		check(module.m_trackStates[0].evolution.pass == 2,
			"automatic arrangement loop advances evolution despite pattern-phase restart");
		module.publishTelemetry(false, 120.f);
		std::string response, error;
		module.handleSibylRequest(SibylControl::Operation::GET_STATUS, "{}", response, error);
		check(response.find("\"evolutionPasses\":{\"voice\":2}") != std::string::npos,
			"status publishes coherent per-track evolution pass");
		check(module.handleSibylRequest(SibylControl::Operation::TRANSPORT,
			R"({"action":"restart","target":"arrangement","apply_at":"immediate"})", response, error),
			"evolution replay reset accepted");
		processOneSample(module);
		check(module.m_trackStates[0].evolution.pass == 0 && module.m_trackStates[0].currentVel == 6.f,
			"explicit arrangement restart restores authored first pass");
		for (int i = 0; i < 25000; ++i) processOneSample(module);
		check(module.m_trackStates[0].currentVel == second.currentVel
			&& module.m_trackStates[0].currentMod[0] == second.currentMod[0]
			&& module.m_trackStates[0].activeEventGate == second.activeEventGate,
			"restart reproduces the same second-pass expression");
	}
	for (bool resetInput : {false, true}) for (bool externalClock : {false, true}) {
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		if (externalClock) module.inputs[SibylModule::CLOCK_INPUT].channels = 1;
		processOneSample(module);
		if (resetInput) {
			module.inputs[SibylModule::RESET_INPUT].channels = 1;
			module.inputs[SibylModule::RESET_INPUT].setVoltage(10.f);
		} else module.params[SibylModule::RESET_BUTTON_PARAM].setValue(1.f);
		processOneSample(module);
		check(module.outputs[SibylModule::EOC_OUTPUT].getVoltage() == 0.f,
			"reset request does not pulse EoC before its clock boundary");
		if (externalClock) module.inputs[SibylModule::CLOCK_INPUT].setVoltage(10.f);
		else module.m_clockBoundaryPhaseBeats = 1.0 - 1e-6;
		processOneSample(module);
		check(module.m_pendingHardwareAction == SibylModule::HardwareAction::NONE &&
			module.outputs[SibylModule::EOC_OUTPUT].getVoltage() == 10.f &&
			module.outputs[SibylModule::SCENE_OUTPUT].getVoltage() == 10.f &&
			module.outputs[SibylModule::CLOCK_OUTPUT].getVoltage() == 10.f,
			"button/input reset pulses EoC with Scene and Clock when applied");
		int highSamples = 1;
		for (int sample = 0; sample < 100; ++sample) {
			processOneSample(module);
			if (module.outputs[SibylModule::EOC_OUTPUT].getVoltage() == 10.f) ++highSamples;
		}
		check(highSamples >= 48 && highSamples <= 49 &&
			module.outputs[SibylModule::EOC_OUTPUT].getVoltage() == 0.f,
			"reset EoC lasts 1ms and a held reset does not retrigger");
	}
	{
		SibylModule module;
		std::shared_ptr<sibyl::Composition> composition = std::const_pointer_cast<sibyl::Composition>(
			makeComposition(1, false));
		sibyl::Scene secondScene = composition->arrangement.front();
		secondScene.id = "second_scene";
		composition->arrangement.push_back(secondScene);
		module.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);

		module.params[SibylModule::SCENE_TRIG_BUTTON_PARAM].setValue(1.f);
		processOneSample(module);
		check(module.m_pendingHardwareAction == SibylModule::HardwareAction::SELECT_SCENE,
			"manual TRIG button requests a scene change through the hardware path");
		check(module.m_pendingHardwareScene == (module.m_sceneIndex + 1) % 2,
			"manual TRIG button targets the next scene");
		module.params[SibylModule::SCENE_TRIG_BUTTON_PARAM].setValue(0.f);
		processOneSample(module);

		module.params[SibylModule::RESET_BUTTON_PARAM].setValue(1.f);
		processOneSample(module);
		check(module.m_pendingHardwareAction == SibylModule::HardwareAction::RESET_ARRANGEMENT,
			"manual RST button requests an arrangement reset through the hardware path");

		const bool initialRunning = module.m_runtimeRunning.load(std::memory_order_acquire);
		module.params[SibylModule::RUN_BUTTON_PARAM].setValue(1.f);
		processOneSample(module);
		check(module.m_runtimeRunning.load(std::memory_order_acquire) != initialRunning,
			"manual RUN button toggles runtime transport");
		module.inputs[SibylModule::RUN_INPUT].channels = 1;
		module.inputs[SibylModule::RUN_INPUT].setVoltage(0.f);
		module.params[SibylModule::RUN_BUTTON_PARAM].setValue(0.f);
		processOneSample(module);
		check(!module.m_effectiveRunning.load(std::memory_order_acquire),
			"patched RUN input retains precedence over the manual button");

		module.params[SibylModule::LOOP_BUTTON_PARAM].setValue(1.f);
		processOneSample(module);
		SibylModule::DisplaySnapshot loopDisplay = module.readDisplaySnapshot();
		check(module.m_loopOverride.load(std::memory_order_acquire) == 1 &&
			loopDisplay.looping && !loopDisplay.loopFollowsComposition,
			"manual CLK loop button advances AUTO to explicit LOOP");
		json_t* loopSaved = module.dataToJson();
		SibylModule loopRestored;
		loopRestored.dataFromJson(loopSaved);
		check(loopRestored.m_loopOverride.load(std::memory_order_acquire) == 1,
			"explicit LOOP override persists with the patch");
		json_decref(loopSaved);
		module.params[SibylModule::LOOP_BUTTON_PARAM].setValue(0.f);
		processOneSample(module);
		module.params[SibylModule::LOOP_BUTTON_PARAM].setValue(1.f);
		processOneSample(module);
		loopDisplay = module.readDisplaySnapshot();
		check(module.m_loopOverride.load(std::memory_order_acquire) == 0 &&
			!loopDisplay.looping && !loopDisplay.loopFollowsComposition,
			"manual CLK loop button advances LOOP to explicit ONCE");
		json_t* saved = module.dataToJson();
		SibylModule restored;
		restored.dataFromJson(saved);
		check(restored.m_loopOverride.load(std::memory_order_acquire) == 0,
			"manual loop override persists with the patch");
		json_decref(saved);
		module.params[SibylModule::LOOP_BUTTON_PARAM].setValue(0.f);
		processOneSample(module);
		module.params[SibylModule::LOOP_BUTTON_PARAM].setValue(1.f);
		processOneSample(module);
		loopDisplay = module.readDisplaySnapshot();
		check(module.m_loopOverride.load(std::memory_order_acquire) == -1 &&
			loopDisplay.looping && loopDisplay.loopFollowsComposition,
			"manual CLK loop button advances ONCE back to AUTO");
	}

	{
		SibylModule module;
		uint64_t cursor = octavia::observationBus().latestSequence();
		const uint64_t requestId = module.publishObservationTrigger(
			9876, 123456, 240, 480, 0x3c, "scene-experiment");
		octavia::ObservationTrigger trigger;
		uint64_t dropped = 0;
		check(requestId != 0 && octavia::observationBus().poll(&cursor, &trigger, &dropped)
			&& trigger.requestId == requestId && trigger.octaviaModuleId == 9876
			&& trigger.triggerFrame == 123456 && trigger.preFrames == 240
			&& trigger.postFrames == 480 && trigger.monitorMask == 0x3c
			&& trigger.labelString() == "scene-experiment" && dropped == 0,
			"Sibyl publishes an allocation-free exact-frame Octavia observation trigger");
	}

	{
		SibylModule module;
		std::shared_ptr<sibyl::Composition> composition =
			std::const_pointer_cast<sibyl::Composition>(makeComposition(2, false));
		sibyl::StepEvent& event = composition->patterns["first"].steps[0];
		event.hasObservation = true;
		event.observation.octaviaModuleId = 2468;
		event.observation.preFrames = 32;
		event.observation.postFrames = 96;
		event.observation.monitorMask = 0x0c;
		event.observation.label = "authored-onset";
		uint64_t cursor = octavia::observationBus().latestSequence();
		module.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		Module::ProcessArgs args;
		args.sampleRate = 48000.f;
		args.sampleTime = 1.f / args.sampleRate;
		args.frame = 555;
		gAllocationCount = 0;
		gTrackAllocations = true;
		module.process(args);
		gTrackAllocations = false;
		octavia::ObservationTrigger trigger;
		uint64_t dropped = 0;
		check(octavia::observationBus().poll(&cursor, &trigger, &dropped)
			&& trigger.octaviaModuleId == 2468 && trigger.triggerFrame == 555
			&& trigger.preFrames == 32 && trigger.postFrames == 96
			&& trigger.monitorMask == 0x0c && trigger.labelString() == "authored-onset",
			"authored observation marker publishes on the exact sounding event frame");
		check(gAllocationCount == 0,
			"authored observation publication performs no audio-thread allocation");

		SibylModule silentModule;
		composition->patterns["first"].steps[0].hasProbability = true;
		composition->patterns["first"].steps[0].probability = 0.f;
		cursor = octavia::observationBus().latestSequence();
		silentModule.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		args.frame = 556;
		silentModule.process(args);
		check(!octavia::observationBus().poll(&cursor, &trigger, &dropped),
			"probability-suppressed event does not publish an observation marker");
	}

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
		check(std::abs(module.outputs[SibylModule::MOD_OUTPUT].getVoltage(0) - (-10.0f)) < 1e-6f &&
			std::abs(module.outputs[SibylModule::MOD_2_OUTPUT].getVoltage(0) - 6.5f) < 1e-6f &&
			std::abs(module.outputs[SibylModule::MOD_3_OUTPUT].getVoltage(0) - 10.0f) < 1e-6f,
			"three independent polyphonic modulation lanes follow authored step values");
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
		module.acceptComposition(makeTimingComposition(0.0f, 0.0f, 1, 3.0f), sibyl::ApplyAt::IMMEDIATE,
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
		for (int i = 0; i < 800; ++i) { args.frame = i; module.process(args); }
		check(module.outputs[SibylModule::GATE_OUTPUT].getVoltage(0) > 9.0f,
			"multi-step gate remains high beyond its originating step");
		for (int i = 800; i < 1050; ++i) { args.frame = i; module.process(args); }
		check(module.outputs[SibylModule::GATE_OUTPUT].getVoltage(0) == 0.0f,
			"multi-step gate closes after its authored duration");
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

	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		gAllocationCount = 0;
		gTrackAllocations = true;
		for (int i = 0; i < 4096; ++i) processOneSample(module);
		gTrackAllocations = false;
		check(gAllocationCount == 0,
			"steady-state Sibyl process performs no dynamic allocation");
	}

	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		SibylModule::DisplaySnapshot display;
		module.readDisplaySnapshot(display);
		gAllocationCount = 0;
		gTrackAllocations = true;
		for (int i = 0; i < 256; ++i) module.readDisplaySnapshot(display);
		gTrackAllocations = false;
		check(gAllocationCount == 0,
			"steady-state Oracle snapshots reuse immutable layout data and string storage");
	}

	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		module.m_telemetrySequence.store(0, std::memory_order_relaxed);
		module.m_telemetryPublishCountdown = 0;
		for (int sample = 0; sample < 800; ++sample) processOneSample(module);
		const uint64_t firstWindowSequence = module.m_telemetrySequence.load(std::memory_order_acquire);
		processOneSample(module);
		const uint64_t secondWindowSequence = module.m_telemetrySequence.load(std::memory_order_acquire);
		check(firstWindowSequence == 2 && secondWindowSequence == 4,
			"display telemetry publishes at 60 Hz instead of on every audio sample");
	}

	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		for (int revision = 2; revision <= 1001; ++revision) {
			module.acceptComposition(makeComposition(revision, false), sibyl::ApplyAt::IMMEDIATE,
				sibyl::PhasePolicy::PRESERVE);
			processOneSample(module);
		}
		std::string response;
		std::string error;
		bool transportsAccepted = true;
		for (int i = 0; i < 1000; ++i) {
			transportsAccepted = module.handleSibylRequest(SibylControl::Operation::TRANSPORT,
				"{\"action\":\"panic\"}", response, error) && transportsAccepted;
			processOneSample(module);
		}
		check(transportsAccepted, "sustained transport requests remain accepted");
		module.reclaimPublishedObjects();
		check(module.m_compositionOwners.size() <= 2 && module.m_adoptionOwners.size() <= 1 &&
			module.m_transportOwners.size() <= 1,
			"sustained edit and transport publication reclaims owner pools to bounded size");
	}

	{
		SibylModule module;
		module.acceptComposition(makeComposition(1, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		std::atomic<bool> stopDsp {false};
		std::thread dspThread([&]() {
			while (!stopDsp.load(std::memory_order_acquire)) processOneSample(module);
		});
		bool coherentReads = true;
		for (int revision = 2; revision <= 1001; ++revision) {
			module.acceptComposition(makeComposition(revision, false), sibyl::ApplyAt::IMMEDIATE,
				sibyl::PhasePolicy::PRESERVE);
			const SibylModule::DisplaySnapshot display = module.readDisplaySnapshot();
			coherentReads = coherentReads && display.acceptedRevision == revision &&
				display.activeRevision >= 1 && display.activeRevision <= revision;
			module.reclaimPublishedObjects();
			if ((revision & 15) == 0) std::this_thread::yield();
		}
		stopDsp.store(true, std::memory_order_release);
		dspThread.join();
		// The control loop must not depend on the host scheduler giving the DSP
		// thread a final timeslice. Consume any last coalesced adoption explicitly.
		processOneSample(module);
		module.reclaimPublishedObjects();
		check(coherentReads && module.m_compositionOwners.size() <= 2 &&
			module.m_adoptionOwners.size() <= 1,
			"concurrent DSP adoption, display reads, and snapshot reclamation remain coherent");
	}

	{
		SibylModule module;
		std::shared_ptr<sibyl::Composition> composition = std::const_pointer_cast<sibyl::Composition>(
			makeComposition(7, false));
		composition->meta.title = "Constellation Engine";
		composition->meta.prompt = "A bright ascending ritual";
		composition->arrangement[0].name = "Invocation";
		composition->arrangement[0].description = "Open slowly, keeping the first voice suspended.";
		composition->arrangement[0].repeats = 3;
		module.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		module.m_trackStates[0].patternPhaseBeats = 0.5;
		module.publishTelemetry(false, 120.0);
		SibylModule::DisplaySnapshot display = module.readDisplaySnapshot();
		check(display.title == "Constellation Engine" &&
			display.prompt == "A bright ascending ritual" && display.scene == "Invocation",
			"oracle snapshot resolves immutable title, prompt, and active scene metadata");
		check(display.sceneIndex == 0 && display.sceneCount == 1,
			"oracle snapshot exposes one-based scene-position source data");
		check(display.sceneDescription == "Open slowly, keeping the first voice suspended.",
			"oracle snapshot exposes the active scene description for the message box");
		check((display.activeTrackMask & 1u) != 0 && display.playhead[0] > 0.49f && display.playhead[0] < 0.51f,
			"oracle snapshot publishes a normalized active-track playhead");
		check(display.acceptedRevision == 7 && display.activeRevision == 7 && display.sceneRepeats == 3,
			"oracle snapshot reports coherent revision and repeat state");
		check(module.m_displayCompositionHazard.load(std::memory_order_acquire) == nullptr,
			"oracle snapshot releases its immutable-composition hazard");
		const SibylModule::VoicingSnapshot voicing = module.readVoicingSnapshot();
		check(voicing.scene == "Invocation" && voicing.rows.size() == 1
			&& voicing.rows[0].channel == 0 && voicing.rows[0].trackId == "voice"
			&& voicing.rows[0].patternId == "first" && voicing.rows[0].pitches.size() == 1
			&& std::abs(voicing.rows[0].pitches[0] - 1.f) < 1e-6f,
			"voicing snapshot exposes each active part's authored pitch material");
		check(module.m_voicingCompositionHazard.load(std::memory_order_acquire) == nullptr,
			"voicing snapshot releases its immutable-composition hazard");
	}

	{
		SibylModule module;
		std::shared_ptr<sibyl::Composition> composition = std::const_pointer_cast<sibyl::Composition>(
			makeComposition(8, false));
		composition->arrangement[0].lengthBeats = 4.f;
		composition->arrangement[0].repeats = 2;
		sibyl::Scene secondScene = composition->arrangement[0];
		secondScene.id = "long_second_scene";
		secondScene.lengthBeats = 8.f;
		secondScene.repeats = 1;
		composition->arrangement.push_back(secondScene);
		module.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		module.m_sceneIndex = 0;
		module.m_sceneRepeat = 1;
		module.m_scenePhase = 2.0;
		module.publishTelemetry(false, 120.0);
		const SibylModule::DisplaySnapshot display = module.readDisplaySnapshot();
		check(display.progressSegmentCount == 2
			&& std::abs(display.progressSegmentEnds[0] - 0.5f) < 1e-6f
			&& std::abs(display.progressSegmentEnds[1] - 1.f) < 1e-6f,
			"oracle arrangement bar weights scene segments by length and repeats");
		check(std::abs(display.sceneProgress - 0.5f) < 1e-6f
			&& std::abs(display.arrangementProgress - 0.375f) < 1e-6f,
			"oracle arrangement bar includes completed repeats and current scene progress");
		check(module.m_displayCompositionHazard.load(std::memory_order_acquire) == nullptr,
			"arrangement progress calculation releases its immutable-composition hazard");
	}

	{
		SibylModule module;
		std::shared_ptr<sibyl::Composition> composition = std::const_pointer_cast<sibyl::Composition>(
			makeComposition(9, false));
		composition->transport.loop = false;
		composition->arrangement[0].repeats = 3;
		module.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(module);
		module.m_sceneRepeat = 2;
		module.m_scenePhase = composition->arrangement[0].lengthBeats - 1e-6;
		processOneSample(module);
		module.publishTelemetry(false, composition->meta.bpm);
		const SibylModule::DisplaySnapshot display = module.readDisplaySnapshot();
		check(!module.m_runtimeRunning.load(std::memory_order_acquire)
			&& display.sceneRepeat == 2
			&& std::abs(display.sceneProgress - 1.f) < 1e-6f
			&& std::abs(display.arrangementProgress - 1.f) < 1e-6f,
			"non-looping repeated finale keeps both progress bars full after stopping");
	}

	{
		SibylModule source;
		source.acceptComposition(makeComposition(20, false), sibyl::ApplyAt::IMMEDIATE,
			sibyl::PhasePolicy::RESTART_ALL);
		processOneSample(source);
		std::shared_ptr<sibyl::Composition> pending = std::const_pointer_cast<sibyl::Composition>(
			makeComposition(21, false));
		pending->meta.title = "Pending reload winner";
		pending->transport.running = false;
		pending->transport.loop = false;
		source.acceptComposition(pending, sibyl::ApplyAt::NEXT_SCENE, sibyl::PhasePolicy::PRESERVE);
		source.m_lastError = "stale transient error";
		source.m_lastWarnings.push_back({"meta", "stale transient warning"});
		json_t* saved = source.dataToJson();
		SibylModule restored;
		restored.dataFromJson(saved);
		json_decref(saved);
		processOneSample(restored);
		const SibylModule::DisplaySnapshot display = restored.readDisplaySnapshot();
		check(restored.m_acceptedRevision == 21 && restored.m_activeRevision.load(std::memory_order_acquire) == 21 &&
			restored.m_acceptedCompositionPtr->meta.title == "Pending reload winner",
			"patch reload restores the newest accepted composition even when it was pending");
		check(!restored.m_runtimeRunning.load(std::memory_order_acquire) && !display.running,
			"patch reload restores a stopped composition");
		check(restored.m_loopOverride.load(std::memory_order_acquire) == -1 &&
			display.loopFollowsComposition && !display.looping,
			"patch reload preserves AUTO and follows a non-looping composition");
		check(restored.m_lastError.empty() && restored.m_lastWarnings.empty(),
			"patch reload derives diagnostics instead of restoring stale serialized status");
	}

	{
		const std::string path = "build/tests/sibyl_portable_composition_spec.json";
		SibylModule source;
		std::shared_ptr<sibyl::Composition> composition = std::const_pointer_cast<sibyl::Composition>(
			makeComposition(12, true));
		composition->meta.title = "Portable Constellation";
		source.acceptComposition(composition, sibyl::ApplyAt::IMMEDIATE, sibyl::PhasePolicy::RESTART_ALL);
		std::string error;
		check(source.saveCompositionToPath(path, &error),
			"portable composition saves as a standalone JSON envelope");

		json_error_t jsonError {};
		json_t* saved = json_load_file(path.c_str(), 0, &jsonError);
		check(saved && json_is_object(saved) &&
			json_is_string(json_object_get(saved, "format")) &&
			std::string(json_string_value(json_object_get(saved, "format"))) == "Leviathan.SibylComposition" &&
			json_integer_value(json_object_get(saved, "schemaVersion")) == 4 &&
			json_is_object(json_object_get(saved, "composition")),
			"portable composition envelope is versioned and self-identifying");
		if (saved) json_decref(saved);

		SibylModule destination;
		check(destination.loadCompositionFromPath(path, &error) &&
			destination.m_acceptedCompositionPtr &&
			destination.m_acceptedCompositionPtr->meta.title == "Portable Constellation",
			"portable composition round-trips through Sibyl validation");
		const int acceptedRevision = destination.m_acceptedRevision;
		const sibyl::Composition* acceptedComposition = destination.m_acceptedCompositionPtr;
		{
			std::ofstream invalid(path.c_str(), std::ios::binary | std::ios::trunc);
			invalid << "{\"format\":\"Leviathan.SibylComposition\",\"schemaVersion\":1,\"composition\":{}}";
		}
		check(!destination.loadCompositionFromPath(path, &error) &&
			destination.m_acceptedRevision == acceptedRevision &&
			destination.m_acceptedCompositionPtr == acceptedComposition,
			"legacy schema v1 composition is rejected without disturbing the accepted sequence");
		{
			std::ofstream invalid(path.c_str(), std::ios::binary | std::ios::trunc);
			invalid << "{\"format\":\"Leviathan.SibylComposition\",\"schemaVersion\":2,";
		}
		check(!destination.loadCompositionFromPath(path, &error) && !error.empty() &&
			destination.m_acceptedRevision == acceptedRevision &&
			destination.m_acceptedCompositionPtr == acceptedComposition,
			"truncated portable JSON is rejected without disturbing the accepted sequence");
		{
			std::ofstream invalid(path.c_str(), std::ios::binary | std::ios::trunc);
			invalid << "{\"format\":\"Another.Sequencer\",\"schemaVersion\":2,\"composition\":{}}";
		}
		check(!destination.loadCompositionFromPath(path, &error) &&
			destination.m_acceptedRevision == acceptedRevision &&
			destination.m_acceptedCompositionPtr == acceptedComposition,
			"foreign portable envelope is rejected without disturbing the accepted sequence");
		{
			std::ofstream invalid(path.c_str(), std::ios::binary | std::ios::trunc);
			invalid << "{\"format\":\"Leviathan.SibylComposition\",\"schemaVersion\":2}";
		}
		check(!destination.loadCompositionFromPath(path, &error) &&
			destination.m_acceptedRevision == acceptedRevision &&
			destination.m_acceptedCompositionPtr == acceptedComposition,
			"portable envelope without a composition is rejected safely");
		{
			std::ofstream oversized(path.c_str(), std::ios::binary | std::ios::trunc);
			oversized.seekp(16 * 1024 * 1024);
			oversized.put('x');
		}
		check(!destination.loadCompositionFromPath(path, &error) &&
			error.find("16 MB") != std::string::npos &&
			destination.m_acceptedRevision == acceptedRevision &&
			destination.m_acceptedCompositionPtr == acceptedComposition,
			"oversized portable composition is rejected before parsing without disturbing playback");
		std::remove(path.c_str());
	}

	std::cout << "[SUMMARY] sibyl_module_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
