#include "../src/Cantor.hpp"

#include <cmath>
#include <iostream>

namespace {

void process(Cantor& module, int64_t frame = 0) {
	Module::ProcessArgs args;
	args.sampleRate = 48000.f;
	args.sampleTime = 1.f / args.sampleRate;
	args.frame = frame;
	module.process(args);
}

bool near(float a, float b, float tolerance = 1e-5f) {
	return std::fabs(a - b) <= tolerance;
}

bool staticFallbackQuantizesPolyphony() {
	Cantor module;
	module.params[Cantor::INTENT_PARAM].setValue(1.f);
	module.params[Cantor::COHERENCE_PARAM].setValue(0.f);
	module.params[Cantor::FIELD_PARAM].setValue(1.f);
	module.inputs[Cantor::PITCH_INPUT].channels = 2;
	const float majorThird = std::log2(5.f / 4.f);
	const float perfectFifth = std::log2(3.f / 2.f);
	module.inputs[Cantor::PITCH_INPUT].setVoltage(majorThird, 0);
	module.inputs[Cantor::PITCH_INPUT].setVoltage(perfectFifth, 1);
	process(module);
	const bool pass = near(module.outputs[Cantor::PITCH_OUTPUT].getVoltage(0), majorThird)
		&& near(module.outputs[Cantor::PITCH_OUTPUT].getVoltage(1), perfectFifth)
		&& module.visualActiveVoices.load(std::memory_order_relaxed) == 0;
	if (!pass) {
		std::cout << "  static detail outputs="
			<< module.outputs[Cantor::PITCH_OUTPUT].getVoltage(0)
			<< "/" << module.outputs[Cantor::PITCH_OUTPUT].getVoltage(1)
			<< " expected=" << majorThird << "/" << perfectFifth << '\n';
	}
	return pass;
}

bool gatesLatchAndReleaseContext() {
	Cantor module;
	module.inputs[Cantor::PITCH_INPUT].channels = 2;
	module.inputs[Cantor::GATE_INPUT].channels = 2;
	module.inputs[Cantor::PITCH_INPUT].setVoltage(0.22f, 0);
	module.inputs[Cantor::PITCH_INPUT].setVoltage(0.68f, 1);
	module.inputs[Cantor::GATE_INPUT].setVoltage(0.f, 0);
	module.inputs[Cantor::GATE_INPUT].setVoltage(0.f, 1);
	process(module, 0);
	module.inputs[Cantor::GATE_INPUT].setVoltage(10.f, 0);
	module.inputs[Cantor::GATE_INPUT].setVoltage(10.f, 1);
	process(module, 1);
	const float held0 = module.outputs[Cantor::PITCH_OUTPUT].getVoltage(0);
	const float held1 = module.outputs[Cantor::PITCH_OUTPUT].getVoltage(1);
	const bool bothActive =
		module.visualActiveVoices.load(std::memory_order_relaxed) == 2;
	module.inputs[Cantor::PITCH_INPUT].setVoltage(-1.5f, 0);
	module.inputs[Cantor::PITCH_INPUT].setVoltage(1.5f, 1);
	process(module, 2);
	const bool held = near(module.outputs[Cantor::PITCH_OUTPUT].getVoltage(0), held0)
		&& near(module.outputs[Cantor::PITCH_OUTPUT].getVoltage(1), held1);
	module.inputs[Cantor::GATE_INPUT].setVoltage(0.f, 0);
	process(module, 3);
	return bothActive && held
		&& module.visualActiveVoices.load(std::memory_order_relaxed) == 1
		&& !module.culture.isVoiceActive(0)
		&& module.culture.isVoiceActive(1);
}

bool monophonicGateBroadcasts() {
	Cantor module;
	module.inputs[Cantor::PITCH_INPUT].channels = 3;
	module.inputs[Cantor::GATE_INPUT].channels = 1;
	module.inputs[Cantor::PITCH_INPUT].setVoltage(0.1f, 0);
	module.inputs[Cantor::PITCH_INPUT].setVoltage(0.4f, 1);
	module.inputs[Cantor::PITCH_INPUT].setVoltage(0.7f, 2);
	module.inputs[Cantor::GATE_INPUT].setVoltage(0.f);
	process(module, 0);
	module.inputs[Cantor::GATE_INPUT].setVoltage(10.f);
	process(module, 1);
	const bool pass = module.visualActiveVoices.load(std::memory_order_relaxed) == 3
		&& module.culture.isVoiceActive(0)
		&& module.culture.isVoiceActive(1)
		&& module.culture.isVoiceActive(2);
	if (!pass) {
		std::cout << "  broadcast detail active="
			<< module.visualActiveVoices.load(std::memory_order_relaxed)
			<< " voices=" << module.culture.isVoiceActive(0)
			<< module.culture.isVoiceActive(1)
			<< module.culture.isVoiceActive(2) << '\n';
	}
	return pass;
}

bool seedStateSerializes() {
	Cantor module;
	module.cultureSeed.store(7331u, std::memory_order_relaxed);
	module.serializedRandomState.store(99173u, std::memory_order_relaxed);
	json_t* root = module.dataToJson();
	Cantor loaded;
	loaded.dataFromJson(root);
	json_decref(root);
	process(loaded);
	return loaded.culture.getSeed() == 7331u
		&& loaded.culture.getRandomState() == 99173u;
}

} // namespace

int main() {
	const bool staticPass = staticFallbackQuantizesPolyphony();
	const bool latchPass = gatesLatchAndReleaseContext();
	const bool broadcastPass = monophonicGateBroadcasts();
	const bool serializationPass = seedStateSerializes();
	std::cout << (staticPass ? "[PASS] " : "[FAIL] ")
		<< "No-gate Culture fallback is static and polyphonic\n";
	std::cout << (latchPass ? "[PASS] " : "[FAIL] ")
		<< "Rising gates latch notes and falling gates release context\n";
	std::cout << (broadcastPass ? "[PASS] " : "[FAIL] ")
		<< "A monophonic gate broadcasts across pitch channels\n";
	std::cout << (serializationPass ? "[PASS] " : "[FAIL] ")
		<< "Culture interpretation seed and RNG state survive serialization\n";
	const int passed = int(staticPass) + int(latchPass)
		+ int(broadcastPass) + int(serializationPass);
	std::cout << "[SUMMARY] cantor_module_spec: " << passed << "/4 passed\n";
	return passed == 4 ? 0 : 1;
}
