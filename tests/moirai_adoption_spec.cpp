#include "MoiraiCompiler.hpp"
#include "MoiraiEngine.hpp"
#include "MoiraiPresets.hpp"

#include <iostream>
#include <atomic>
#include <thread>

namespace {
int failures = 0;
void check(bool value, const char* name) {
	std::cout << (value ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!value) ++failures;
}

moirai::CompiledBankPtr generation(int revision, moirai::OutputMode mode) {
	moirai::Bank bank = moirai::makeInitialBank();
	bank.revision = revision;
	bank.lanes[0].outputMode = mode;
	bank.lanes[1].outputMode = mode;
	return moirai::compileBank(bank).bank;
}

void process(moirai::Engine& engine, moirai::EngineInputs& inputs,
		moirai::EngineOutputs& outputs, float gate, bool clock = false) {
	inputs.gate[0] = gate;
	inputs.clockEdge = clock;
	engine.process(inputs, outputs);
	inputs.clockEdge = false;
}
}

int main() {
	moirai::EngineInputs inputs;
	inputs.channels = 1;
	inputs.sampleTime = 0.001f;
	inputs.velocity[0] = 10.f;
	moirai::EngineOutputs outputs;

	{
		moirai::Engine engine;
		auto first = generation(1, moirai::OutputMode::UNIPOLAR_10);
		auto second = generation(2, moirai::OutputMode::UNIPOLAR_5);
		engine.installBank(first);
		engine.acceptBank(second, moirai::ApplyAt::NEXT_TRIGGER,
			moirai::ActiveVoicePolicy::FINISH_CURRENT);
		process(engine, inputs, outputs, 0.f);
		check(engine.activeRevision() == 1 && engine.pendingRevision() == 2,
			"nextTrigger remains pending without a gate rise");
		process(engine, inputs, outputs, 10.f);
		check(engine.activeRevision() == 2 && engine.pendingRevision() == -1,
			"nextTrigger adopts before voices start on the rising sample");
	}

	{
		moirai::Engine engine;
		auto first = generation(1, moirai::OutputMode::UNIPOLAR_10);
		auto second = generation(2, moirai::OutputMode::UNIPOLAR_5);
		engine.installBank(first);
		process(engine, inputs, outputs, 0.f);
		process(engine, inputs, outputs, 10.f);
		for (int i = 0; i < 20; ++i) process(engine, inputs, outputs, 10.f);
		engine.acceptBank(second, moirai::ApplyAt::IMMEDIATE,
			moirai::ActiveVoicePolicy::FINISH_CURRENT);
		process(engine, inputs, outputs, 10.f);
		check(engine.activeRevision() == 2 && engine.voice(0, 0).bank == first.get()
			&& first->activeVoiceCount.load() == 2,
			"finishCurrent adopts for new triggers while both running lane voices retain their generation");
		process(engine, inputs, outputs, 0.f);
		for (int i = 0; i < 2000 && engine.voice(0, 0).running; ++i)
			process(engine, inputs, outputs, 0.f);
		check(first->activeVoiceCount.load() == 0,
			"voice completion releases the old generation exactly once");
		engine.reclaimGenerations();
		check(engine.ownedGenerationCount() == 1,
			"control-side reclamation releases an inactive unreferenced generation");
	}

	{
		moirai::Engine engine;
		auto first = generation(1, moirai::OutputMode::UNIPOLAR_10);
		auto second = generation(2, moirai::OutputMode::UNIPOLAR_5);
		engine.installBank(first);
		process(engine, inputs, outputs, 0.f);
		process(engine, inputs, outputs, 10.f);
		engine.acceptBank(second, moirai::ApplyAt::ALL_IDLE,
			moirai::ActiveVoicePolicy::FINISH_CURRENT);
		process(engine, inputs, outputs, 10.f);
		check(engine.activeRevision() == 1, "allIdle waits while any lane voice runs");
		process(engine, inputs, outputs, 0.f);
		for (int i = 0; i < 3000 && engine.activeRevision() == 1; ++i)
			process(engine, inputs, outputs, 0.f);
		check(engine.activeRevision() == 2, "allIdle adopts after every voice becomes idle");
	}

	{
		moirai::Engine engine;
		auto first = generation(1, moirai::OutputMode::UNIPOLAR_10);
		auto second = generation(2, moirai::OutputMode::UNIPOLAR_5);
		engine.installBank(first);
		engine.acceptBank(second, moirai::ApplyAt::NEXT_CLOCK,
			moirai::ActiveVoicePolicy::FINISH_CURRENT);
		process(engine, inputs, outputs, 0.f, false);
		check(engine.activeRevision() == 1, "nextClock ignores ordinary samples");
		process(engine, inputs, outputs, 0.f, true);
		check(engine.activeRevision() == 2, "nextClock adopts on the clock-edge sample");
	}

	{
		moirai::Engine engine;
		auto first = generation(1, moirai::OutputMode::UNIPOLAR_10);
		auto second = generation(2, moirai::OutputMode::UNIPOLAR_5);
		auto third = generation(3, moirai::OutputMode::BIPOLAR_5);
		engine.installBank(first);
		engine.acceptBank(second, moirai::ApplyAt::NEXT_TRIGGER,
			moirai::ActiveVoicePolicy::FINISH_CURRENT);
		engine.acceptBank(third, moirai::ApplyAt::NEXT_TRIGGER,
			moirai::ActiveVoicePolicy::FINISH_CURRENT);
		process(engine, inputs, outputs, 0.f);
		process(engine, inputs, outputs, 10.f);
		engine.reclaimGenerations();
		check(engine.activeRevision() == 3 && engine.pendingRevision() == -1
			&& engine.ownedGenerationCount() <= 2,
			"coalesced pending edits adopt only the newest generation and reclaim superseded owners");
	}

	{
		moirai::Engine engine;
		auto first = generation(1, moirai::OutputMode::UNIPOLAR_10);
		auto second = generation(2, moirai::OutputMode::UNIPOLAR_5);
		engine.installBank(first);
		process(engine, inputs, outputs, 0.f);
		process(engine, inputs, outputs, 10.f);
		engine.acceptBank(second, moirai::ApplyAt::IMMEDIATE,
			moirai::ActiveVoicePolicy::RESTART_ACTIVE);
		process(engine, inputs, outputs, 10.f);
		check(engine.activeRevision() == 2 && engine.voice(0, 0).bank == second.get()
			&& first->activeVoiceCount.load() == 0
			&& second->activeVoiceCount.load() == 2,
			"immediate restartActive transfers running voices to corresponding programs in the new generation");
	}

	{
		moirai::Engine engine;
		engine.installBank(generation(1, moirai::OutputMode::UNIPOLAR_10));
		std::atomic<bool> stop {false};
		std::thread dsp([&]() {
			moirai::EngineInputs threadedInputs;
			threadedInputs.channels = 1;
			threadedInputs.sampleTime = 0.001f;
			threadedInputs.velocity[0] = 10.f;
			moirai::EngineOutputs threadedOutputs;
			int sample = 0;
			while (!stop.load(std::memory_order_acquire)) {
				threadedInputs.gate[0] = (sample++ & 7) < 4 ? 10.f : 0.f;
				engine.process(threadedInputs, threadedOutputs);
			}
		});
		bool coherent = true;
		for (int revision = 2; revision <= 300; ++revision) {
			engine.acceptBank(generation(revision,
				(revision & 1) ? moirai::OutputMode::UNIPOLAR_10
					: moirai::OutputMode::UNIPOLAR_5),
				moirai::ApplyAt::IMMEDIATE,
				moirai::ActiveVoicePolicy::FINISH_CURRENT);
			engine.reclaimGenerations();
			const int active = engine.activeRevision();
			coherent = coherent && active >= 1 && active <= revision;
		}
		stop.store(true, std::memory_order_release);
		dsp.join();
		process(engine, inputs, outputs, 0.f);
		engine.reset();
		engine.reclaimGenerations();
		check(coherent && engine.activeRevision() == 300
			&& engine.ownedGenerationCount() <= 1,
			"concurrent adoption and control-side reclamation retain every active, pending, hazard, and voice generation");
	}

	std::cout << "[SUMMARY] moirai_adoption_spec: " << (failures ? "FAILED" : "passed") << "\n";
	return failures == 0 ? 0 : 1;
}
