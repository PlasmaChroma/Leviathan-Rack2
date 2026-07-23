#include "../src/Doorstop.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Result {
	std::string name;
	bool pass = false;
	std::string detail;
};

Module::ProcessArgs processArgs(float sampleRate = 48000.f) {
	Module::ProcessArgs args;
	args.sampleRate = sampleRate;
	args.sampleTime = 1.f / sampleRate;
	return args;
}

void armTriggers(Doorstop& module, const Module::ProcessArgs& args) {
	module.inputs[Doorstop::TRIG_INPUT].setVoltage(0.f);
	module.params[Doorstop::MANUAL_PARAM].setValue(0.f);
	module.process(args);
}

Result interfaceContract() {
	Doorstop module;
	const bool pass = module.getNumParams() == Doorstop::PARAMS_LEN
		&& module.getNumInputs() == Doorstop::INPUTS_LEN
		&& module.getNumOutputs() == Doorstop::OUTPUTS_LEN
		&& module.getNumLights() == Doorstop::LIGHTS_LEN;
	return {"Module exposes the fixed MVP interface", pass,
		"params=" + std::to_string(module.getNumParams())
		+ " inputs=" + std::to_string(module.getNumInputs())
		+ " outputs=" + std::to_string(module.getNumOutputs())};
}

Result heldGateStrikesOnce() {
	Doorstop module;
	const auto args = processArgs();
	armTriggers(module, args);
	module.inputs[Doorstop::TRIG_INPUT].setVoltage(10.f);
	module.process(args);
	const float initialLight = module.visualStrike.load(std::memory_order_relaxed);
	for (int i = 0; i < 100; ++i) {
		module.process(args);
	}
	const float laterLight = module.visualStrike.load(std::memory_order_relaxed);
	const float breakIn = module.serializedBreakIn.load(std::memory_order_relaxed);
	const bool pass = initialLight > 0.f && laterLight < initialLight
		&& breakIn > 0.f;
	return {"Held gate produces one decaying strike", pass,
		"initialLight=" + std::to_string(initialLight)
		+ " laterLight=" + std::to_string(laterLight)
		+ " breakIn=" + std::to_string(breakIn)};
}

Result zeroVelocityIsNoOp() {
	Doorstop module;
	const auto args = processArgs();
	armTriggers(module, args);
	module.inputs[Doorstop::VELOCITY_INPUT].channels = 1;
	module.inputs[Doorstop::VELOCITY_INPUT].setVoltage(0.f);
	module.inputs[Doorstop::TRIG_INPUT].setVoltage(10.f);
	module.process(args);
	const bool pass = module.engine.isSleeping()
		&& module.outputs[Doorstop::AUDIO_OUTPUT].getVoltage() == 0.f
		&& module.visualStrike.load(std::memory_order_relaxed) == 0.f;
	return {"Patched zero velocity is a complete no-op", pass,
		"sleeping=" + std::to_string(module.engine.isSleeping())};
}

Result negativeVelocityReversesStrike() {
	Doorstop module;
	const auto args = processArgs();
	armTriggers(module, args);
	module.inputs[Doorstop::VELOCITY_INPUT].channels = 1;
	module.inputs[Doorstop::VELOCITY_INPUT].setVoltage(-5.f);
	module.inputs[Doorstop::TRIG_INPUT].setVoltage(10.f);
	module.process(args);
	const float displacement = module.visualDisplacement.load(std::memory_order_relaxed);
	const float velocity = module.visualVelocity.load(std::memory_order_relaxed);
	const float light = module.visualStrike.load(std::memory_order_relaxed);
	const bool pass = !module.engine.isSleeping()
		&& displacement < 0.f && velocity < 0.f && light > 0.f;
	return {"Negative velocity strikes in the opposite direction", pass,
		"displacement=" + std::to_string(displacement)
		+ " velocity=" + std::to_string(velocity)};
}

Result manualStrikeWorks() {
	Doorstop module;
	const auto args = processArgs();
	armTriggers(module, args);
	module.params[Doorstop::MANUAL_PARAM].setValue(1.f);
	module.process(args);
	const float light = module.lights[Doorstop::STRIKE_LIGHT].getBrightness();
	const float visual = module.visualStrike.load(std::memory_order_relaxed);
	const bool pass = !module.engine.isSleeping() && light > 0.f && visual > 0.f
		&& std::isfinite(module.outputs[Doorstop::AUDIO_OUTPUT].getVoltage());
	return {"Manual parameter strikes audio, light, and telemetry", pass,
		"light=" + std::to_string(light) + " visual=" + std::to_string(visual)};
}

Result manualStrikeHeightControlsVelocity() {
	Doorstop mild;
	Doorstop intense;
	const auto args = processArgs();
	armTriggers(mild, args);
	armTriggers(intense, args);
	mild.pendingManualVelocity.store(0.1f, std::memory_order_relaxed);
	mild.manualVelocityPending.store(true, std::memory_order_release);
	intense.pendingManualVelocity.store(1.f, std::memory_order_relaxed);
	intense.manualVelocityPending.store(true, std::memory_order_release);
	mild.params[Doorstop::MANUAL_PARAM].setValue(1.f);
	intense.params[Doorstop::MANUAL_PARAM].setValue(1.f);
	mild.process(args);
	intense.process(args);
	const float mildDisplacement = std::fabs(
		mild.visualDisplacement.load(std::memory_order_relaxed));
	const float intenseDisplacement = std::fabs(
		intense.visualDisplacement.load(std::memory_order_relaxed));
	const bool pass = intenseDisplacement > mildDisplacement * 10.f
		&& !mild.manualVelocityPending.load(std::memory_order_relaxed)
		&& !intense.manualVelocityPending.load(std::memory_order_relaxed);
	return {"Manual click height controls exactly one strike velocity", pass,
		"mild=" + std::to_string(mildDisplacement)
			+ " intense=" + std::to_string(intenseDisplacement)};
}

Result jsonRoundTripAndReset() {
	Doorstop source;
	source.allowVisualOverflow.store(false, std::memory_order_relaxed);
	source.soundModel.store(int(doorstop::SoundModel::DispersiveSpring), std::memory_order_relaxed);
	source.serializedBreakIn.store(0.37f, std::memory_order_relaxed);
	source.breakInLocked.store(true, std::memory_order_relaxed);
	json_t* rootJ = source.dataToJson();
	Doorstop loaded;
	loaded.dataFromJson(rootJ);
	json_decref(rootJ);
	const auto args = processArgs();
	loaded.process(args);
	const bool restored = !loaded.allowVisualOverflow.load(std::memory_order_relaxed)
		&& loaded.soundModel.load(std::memory_order_relaxed)
			== int(doorstop::SoundModel::DispersiveSpring)
		&& loaded.engine.getSoundModel() == doorstop::SoundModel::DispersiveSpring
		&& std::fabs(loaded.engine.getBreakIn() - 0.37f) < 1e-6f
		&& loaded.engine.isBreakInLocked()
		&& loaded.breakInLocked.load(std::memory_order_relaxed);
	Module::ResetEvent event;
	loaded.onReset(event);
	const bool reset = loaded.allowVisualOverflow.load(std::memory_order_relaxed)
		&& loaded.soundModel.load(std::memory_order_relaxed)
			== int(doorstop::SoundModel::ProbabilisticMix)
		&& loaded.engine.isSleeping()
		&& loaded.engine.getBreakIn() == 0.f
		&& !loaded.engine.isBreakInLocked()
		&& loaded.serializedBreakIn.load(std::memory_order_relaxed) == 0.f
		&& !loaded.breakInLocked.load(std::memory_order_relaxed)
		&& loaded.visualDisplacement.load(std::memory_order_relaxed) == 0.f;
	return {"Configuration JSON round-trips and reset restores defaults", restored && reset,
		"restored=" + std::to_string(restored) + " reset=" + std::to_string(reset)};
}

Result oldPatchAndRestoreCommand() {
	Doorstop module;
	json_t* oldPatchJ = json_object();
	module.dataFromJson(oldPatchJ);
	json_decref(oldPatchJ);
	const auto args = processArgs();
	module.process(args);
	const bool oldPatchFresh = module.engine.getBreakIn() == 0.f
		&& !module.engine.isBreakInLocked();

	module.pendingBreakIn.store(0.65f, std::memory_order_relaxed);
	module.serializedBreakIn.store(0.65f, std::memory_order_relaxed);
	module.breakInLocked.store(true, std::memory_order_relaxed);
	module.breakInStatePending.store(true, std::memory_order_release);
	module.process(args);
	module.restoreSpringRequested.store(true, std::memory_order_release);
	module.process(args);
	const bool restored = module.engine.getBreakIn() == 0.f
		&& module.engine.isBreakInLocked()
		&& module.serializedBreakIn.load(std::memory_order_relaxed) == 0.f;

	module.engine.strike(1.f);
	const bool remainsFreshWhileLocked = module.engine.getBreakIn() == 0.f;
	return {"Old patches default fresh and restore commands preserve the lock",
		oldPatchFresh && restored && remainsFreshWhileLocked,
		"oldPatch=" + std::to_string(oldPatchFresh)
			+ " restored=" + std::to_string(restored)
			+ " lockedFresh=" + std::to_string(remainsFreshWhileLocked)};
}

} // namespace

int main() {
	std::vector<Result> results;
	results.push_back(interfaceContract());
	results.push_back(heldGateStrikesOnce());
	results.push_back(zeroVelocityIsNoOp());
	results.push_back(negativeVelocityReversesStrike());
	results.push_back(manualStrikeWorks());
	results.push_back(manualStrikeHeightControlsVelocity());
	results.push_back(jsonRoundTripAndReset());
	results.push_back(oldPatchAndRestoreCommand());

	int failed = 0;
	std::cout << "Doorstop Runtime Spec\n";
	std::cout << "---------------------\n";
	for (const Result& result : results) {
		std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name
			<< " :: " << result.detail << "\n";
		if (!result.pass) failed++;
	}
	std::cout << "---------------------\n";
	std::cout << "Summary: " << (results.size() - failed) << "/" << results.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}
