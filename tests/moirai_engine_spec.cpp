#include "MoiraiCompiler.hpp"
#include "MoiraiEngine.hpp"
#include "MoiraiPresets.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <new>

namespace {
bool gTrackAllocations = false;
std::size_t gAllocationCount = 0;
}

void* operator new(std::size_t size) {
	if (gTrackAllocations) ++gAllocationCount;
	if (void* memory = std::malloc(size)) return memory;
	throw std::bad_alloc();
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete[](void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }
void operator delete[](void* memory, std::size_t) noexcept { std::free(memory); }

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}

moirai::EngineInputs inputs(float sampleTime = 1.f / 48000.f) {
	moirai::EngineInputs value;
	value.sampleTime = sampleTime;
	value.channels = 1;
	value.velocity.fill(10.f);
	return value;
}

moirai::CompiledBankPtr compile(const moirai::Bank& bank) {
	const moirai::CompileResult result = moirai::compileBank(bank);
	if (!result.valid) {
		for (const moirai::ValidationIssue& issue : result.errors)
			std::cerr << issue.path << ": " << issue.message << "\n";
	}
	return result.bank;
}

moirai::Bank bankFor(const char* presetId) {
	const moirai::Program* preset = moirai::findFactoryProgram(presetId);
	moirai::Bank bank;
	if (preset) bank.programs[preset->id] = *preset;
	for (moirai::Lane& lane : bank.lanes) lane.defaultProgram = presetId;
	return bank;
}
}

int main() {
	{
		const moirai::CompiledBankPtr bank = compile(bankFor("factory_adsr"));
		moirai::Engine engine;
		engine.setBank(bank.get());
		moirai::EngineOutputs output;
		moirai::EngineInputs frame = inputs(0.004f);
		frame.gate[0] = 10.f;
		engine.process(frame, output);
		check(output.envelope[0][0] > 0.f && output.envelope[0][0] < 10.f,
			"staged gate envelope advances through attack");
		const float attackValue = output.envelope[0][0];
		frame.gate[0] = 0.f;
		frame.sampleTime = 0.09f;
		engine.process(frame, output);
		check(output.envelope[0][0] < attackValue && engine.voice(0, 0).releasing,
			"gate-low during attack branches immediately from current value into release");
		frame.sampleTime = 0.2f;
		engine.process(frame, output);
		check(!engine.voice(0, 0).running && output.eoc[0][0] && output.envelope[0][0] == 0.f,
			"release completion idles the voice and emits program EOC");
	}

	{
		const moirai::CompiledBankPtr bank = compile(bankFor("factory_ad_percussive"));
		moirai::Engine engine;
		engine.setBank(bank.get());
		moirai::EngineOutputs output;
		moirai::EngineInputs frame = inputs(0.002f);
		frame.gate[0] = 10.f;
		engine.process(frame, output);
		frame.gate[0] = 0.f;
		engine.process(frame, output);
		check(engine.voice(0, 0).running && output.envelope[0][0] > 9.9f,
			"staged one-shot ignores gate-low and completes its attack");
		frame.sampleTime = 0.25f;
		engine.process(frame, output);
		check(!engine.voice(0, 0).running && output.eoc[0][0],
			"staged one-shot concatenates gate and release paths to completion");
	}

	{
		moirai::Bank authored = bankFor("factory_cycle_triangle");
		authored.lanes[0].eocSource = moirai::EocSource::LOOP;
		const moirai::CompiledBankPtr bank = compile(authored);
		moirai::Engine engine;
		engine.setBank(bank.get());
		moirai::EngineOutputs output;
		moirai::EngineInputs frame = inputs(0.5f);
		frame.bpm = 120.f;
		frame.gate[0] = 10.f;
		engine.process(frame, output);
		check(engine.voice(0, 0).running && output.eoc[0][0],
			"beat-relative cycle wraps at completion and emits loop EOC");
		frame.sampleTime = 0.125f;
		engine.process(frame, output);
		check(std::abs(engine.voice(0, 0).segmentPhase - 0.25f) < 1e-5f &&
			output.envelope[0][0] > 0.f && output.envelope[0][0] < 10.f,
			"cycling contour resumes from its first point after wrapping");
	}

	{
		moirai::Bank authored = bankFor("factory_ar");
		authored.lanes[0].outputMode = moirai::OutputMode::UNIPOLAR_5;
		authored.lanes[1].outputMode = moirai::OutputMode::BIPOLAR_5;
		const moirai::CompiledBankPtr bank = compile(authored);
		moirai::Engine engine;
		engine.setBank(bank.get());
		moirai::EngineOutputs output;
		moirai::EngineInputs frame = inputs(0.012f);
		frame.gate[0] = 10.f;
		frame.panelLevel = 0.5f;
		engine.process(frame, output);
		check(std::abs(output.envelope[0][0] - 2.5f) < 1e-4f,
			"0-5 V mode applies panel level after normalized program output");
		check(std::abs(output.envelope[1][0] - 2.5f) < 1e-4f,
			"bipolar mode scales around 0 V rather than around -5 V");
	}

	{
		const moirai::CompiledBankPtr bank = compile(bankFor("factory_pluck"));
		moirai::Engine engine;
		engine.setBank(bank.get());
		moirai::EngineOutputs output;
		moirai::EngineInputs frame = inputs(0.001f);
		frame.channels = 16;
		for (int channel = 0; channel < 16; ++channel)
			frame.gate[channel] = (channel & 1) ? 10.f : 0.f;
		engine.process(frame, output);
		bool independent = output.channels == 16;
		for (int channel = 0; channel < 16; ++channel) {
			independent = independent && (engine.voice(0, channel).running == ((channel & 1) != 0));
			independent = independent && (engine.voice(1, channel).running == ((channel & 1) != 0));
		}
		check(independent, "both lanes maintain sixteen independent channel voices");
	}

	{
		moirai::Bank authored = bankFor("factory_adsr");
		moirai::Program& program = authored.programs["factory_adsr"];
		program.gatePath[0].loopStart = true;
		program.gatePath[1].loopEnd.mode = moirai::LoopMode::COUNTED;
		program.gatePath[1].loopEnd.count = 2;
		authored.lanes[0].eocSource = moirai::EocSource::LOOP;
		const moirai::CompiledBankPtr bank = compile(authored);
		moirai::Engine engine;
		engine.setBank(bank.get());
		moirai::EngineOutputs output;
		moirai::EngineInputs frame = inputs(0.104f);
		frame.gate[0] = 10.f;
		engine.process(frame, output);
		check(output.eoc[0][0] && engine.voice(0, 0).running && engine.voice(0, 0).loopIteration == 1,
			"counted staged loop reports its boundary and begins its final iteration");
		frame.sampleTime = 0.103f;
		engine.process(frame, output);
		check(output.eoc[0][0] && engine.voice(0, 0).running && engine.voice(0, 0).segment >= 2,
			"counted loop exits forward after its contracted number of passes");
	}

	{
		const moirai::CompiledBankPtr bank = compile(bankFor("factory_pad"));
		moirai::Engine engine;
		engine.setBank(bank.get());
		moirai::EngineOutputs output;
		moirai::EngineInputs frame = inputs();
		frame.channels = 16;
		frame.gate.fill(10.f);
		engine.process(frame, output);
		gAllocationCount = 0;
		gTrackAllocations = true;
		for (int sample = 0; sample < 4096; ++sample) engine.process(frame, output);
		gTrackAllocations = false;
		check(gAllocationCount == 0, "steady-state 32-voice processing performs no dynamic allocation");
	}

	std::cout << (failures ? "[SUMMARY] moirai_engine_spec: FAILED\n" : "[SUMMARY] moirai_engine_spec: passed\n");
	return failures ? 1 : 0;
}
