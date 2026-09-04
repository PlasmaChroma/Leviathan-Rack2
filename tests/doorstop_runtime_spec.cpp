#include "../src/Doorstop.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

bool isDragonKingDebugEnabled() {
	return false;
}

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
	const float visual = module.visualStrike.load(std::memory_order_relaxed);
	const bool pass = !module.engine.isSleeping() && visual > 0.f
		&& std::isfinite(module.outputs[Doorstop::AUDIO_OUTPUT].getVoltage());
	return {"Manual parameter strikes audio and visual telemetry", pass,
		"visual=" + std::to_string(visual)};
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

Result experimentalV3TuningsRoundTrip() {
	bool pass = true;
	for (auto tuning : {doorstop::HelicalTuningVariant::DeepShortTail,
		doorstop::HelicalTuningVariant::DeepBodyBend,
		doorstop::HelicalTuningVariant::DeepThickSpring}) {
		Doorstop source;
		source.engineMode.store(int(doorstop::EngineMode::ReferenceV3));
		source.referenceV3Tuning.store(int(tuning));
		json_t* saved = source.dataToJson();
		Doorstop loaded;
		loaded.dataFromJson(saved);
		json_decref(saved);
		loaded.process(processArgs());
		pass = pass && loaded.referenceV3Tuning.load() == int(tuning)
			&& loaded.engine.getReferenceV3TuningVariant() == tuning
			&& loaded.engine.getEngineMode() == doorstop::EngineMode::ReferenceV3;
	}
	return {"Experimental V3 menu choices survive patch reload", pass,
		"restored=" + std::to_string(pass)};
}

Result jsonRoundTripAndReset() {
	Doorstop source;
	source.allowVisualOverflow.store(false, std::memory_order_relaxed);
	source.engineMode.store(int(doorstop::EngineMode::Legacy), std::memory_order_relaxed);
	source.soundModel.store(int(doorstop::SoundModel::DispersiveSpring), std::memory_order_relaxed);
	source.referenceV3Tuning.store(
		int(doorstop::HelicalTuningVariant::DeepContinuum), std::memory_order_relaxed);
	source.specimenSeed.store(0x12345678u, std::memory_order_relaxed);
	source.serializedBreakIn.store(0.37f, std::memory_order_relaxed);
	source.breakInLocked.store(true, std::memory_order_relaxed);
	json_t* rootJ = source.dataToJson();
	Doorstop loaded;
	loaded.dataFromJson(rootJ);
	json_decref(rootJ);
	const auto args = processArgs();
	loaded.process(args);
	const bool restored = !loaded.allowVisualOverflow.load(std::memory_order_relaxed)
		&& loaded.engineMode.load(std::memory_order_relaxed)
			== int(doorstop::EngineMode::Legacy)
		&& loaded.soundModel.load(std::memory_order_relaxed)
			== int(doorstop::SoundModel::DispersiveSpring)
		&& loaded.referenceV3Tuning.load(std::memory_order_relaxed)
			== int(doorstop::HelicalTuningVariant::DeepContinuum)
		&& loaded.engine.getReferenceV3TuningVariant()
			== doorstop::HelicalTuningVariant::DeepContinuum
		&& loaded.specimenSeed.load(std::memory_order_relaxed) == 0x12345678u
		&& loaded.engine.getEngineMode() == doorstop::EngineMode::Legacy
		&& loaded.engine.getSoundModel() == doorstop::SoundModel::DispersiveSpring
		&& std::fabs(loaded.engine.getBreakIn() - 0.37f) < 1e-6f
		&& loaded.engine.isBreakInLocked()
		&& loaded.breakInLocked.load(std::memory_order_relaxed);
	Module::ResetEvent event;
	loaded.onReset(event);
	const bool reset = loaded.allowVisualOverflow.load(std::memory_order_relaxed)
		&& loaded.engineMode.load(std::memory_order_relaxed)
			== int(doorstop::EngineMode::ReferenceV1)
		&& loaded.soundModel.load(std::memory_order_relaxed)
			== int(doorstop::SoundModel::ProbabilisticMix)
		&& loaded.referenceV3Tuning.load(std::memory_order_relaxed)
			== int(doorstop::HelicalTuningVariant::BoingProbe)
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
		&& !module.engine.isBreakInLocked()
		&& module.engine.getEngineMode() == doorstop::EngineMode::Legacy;

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

Result malformedBreakInJsonIsSafe() {
	const auto args = processArgs();

	Doorstop aboveRange;
	json_t* aboveJ = json_object();
	json_object_set_new(aboveJ, "breakIn", json_real(4.5));
	aboveRange.dataFromJson(aboveJ);
	json_decref(aboveJ);
	aboveRange.process(args);
	const bool clampedHigh = aboveRange.engine.getBreakIn() == 1.f
		&& !aboveRange.engine.isBreakInLocked();

	Doorstop belowRange;
	json_t* belowJ = json_object();
	json_object_set_new(belowJ, "breakIn", json_real(-2.0));
	json_object_set_new(belowJ, "breakInLocked", json_true());
	belowRange.dataFromJson(belowJ);
	json_decref(belowJ);
	belowRange.process(args);
	const bool clampedLow = belowRange.engine.getBreakIn() == 0.f
		&& belowRange.engine.isBreakInLocked();

	Doorstop wrongTypes;
	json_t* wrongJ = json_object();
	json_object_set_new(wrongJ, "breakIn", json_string("old"));
	json_object_set_new(wrongJ, "breakInLocked", json_integer(1));
	json_object_set_new(wrongJ, "referenceV3Tuning", json_string("unknown"));
	wrongTypes.dataFromJson(wrongJ);
	json_decref(wrongJ);
	wrongTypes.process(args);
	const bool wrongTypesDefault = wrongTypes.engine.getBreakIn() == 0.f
		&& !wrongTypes.engine.isBreakInLocked()
		&& wrongTypes.engine.getReferenceV3TuningVariant()
			== doorstop::HelicalTuningVariant::BoingProbe;

	Doorstop nullRoot;
	nullRoot.engine.setBreakIn(0.8f);
	nullRoot.engine.setBreakInLocked(true);
	nullRoot.dataFromJson(nullptr);
	nullRoot.process(args);
	const bool nullDefaults = nullRoot.engine.getBreakIn() == 0.f
		&& !nullRoot.engine.isBreakInLocked()
		&& nullRoot.engine.getEngineMode() == doorstop::EngineMode::Legacy;

	const bool pass = clampedHigh && clampedLow
		&& wrongTypesDefault && nullDefaults;
	return {"Malformed and out-of-range break-in JSON restores safe defaults",
		pass,
		"high=" + std::to_string(clampedHigh)
			+ " low=" + std::to_string(clampedLow)
			+ " types=" + std::to_string(wrongTypesDefault)
			+ " null=" + std::to_string(nullDefaults)};
}

Result newModuleAndSpecimenSemantics() {
	Doorstop module;
	const auto args = processArgs();
	module.process(args);
	const std::uint32_t originalSeed =
		module.specimenSeed.load(std::memory_order_relaxed);
	const bool newDefault =
		module.engine.getEngineMode() == doorstop::EngineMode::ReferenceV1
		&& originalSeed != 0u;

	module.pendingSpecimenSeed.store(0xabcdef01u, std::memory_order_relaxed);
	module.newSpecimenRequested.store(true, std::memory_order_release);
	module.process(args);
	const bool regenerated =
		module.specimenSeed.load(std::memory_order_relaxed) == 0xabcdef01u
		&& module.engine.getSpecimenSeed() == 0xabcdef01u
		&& module.engine.getBreakIn() == 0.f
		&& module.engine.isSleeping();
	return {"New modules use Reference and specimen regeneration is explicit",
		newDefault && regenerated,
		"default=" + std::to_string(newDefault)
			+ " regenerated=" + std::to_string(regenerated)};
}

Result schemaMigrationMapsEveryLegacyModel() {
	const auto args = processArgs();
	bool pass = true;
	for (int model = int(doorstop::SoundModel::Classic);
		model < int(doorstop::SoundModel::Count); ++model) {
		Doorstop module;
		json_t* oldPatchJ = json_object();
		json_object_set_new(oldPatchJ, "schema", json_integer(1));
		json_object_set_new(oldPatchJ, "soundModel", json_integer(model));
		module.dataFromJson(oldPatchJ);
		json_decref(oldPatchJ);
		module.process(args);
		pass = pass
			&& module.engine.getEngineMode() == doorstop::EngineMode::Legacy
			&& int(module.engine.getSoundModel()) == model;
	}

	Doorstop reference;
	reference.engineMode.store(
		int(doorstop::EngineMode::ReferenceV1), std::memory_order_relaxed);
	reference.specimenSeed.store(0x13579bdfu, std::memory_order_relaxed);
	json_t* referenceJ = reference.dataToJson();
	Doorstop loaded;
	loaded.dataFromJson(referenceJ);
	json_decref(referenceJ);
	loaded.process(args);
	const bool referenceRestored =
		loaded.engine.getEngineMode() == doorstop::EngineMode::ReferenceV1
		&& loaded.engine.getSpecimenSeed() == 0x13579bdfu;

	Doorstop referenceV2;
	referenceV2.engineMode.store(
		int(doorstop::EngineMode::ReferenceV2), std::memory_order_relaxed);
	referenceV2.specimenSeed.store(0x2468ace0u, std::memory_order_relaxed);
	json_t* referenceV2J = referenceV2.dataToJson();
	Doorstop loadedV2;
	loadedV2.dataFromJson(referenceV2J);
	json_decref(referenceV2J);
	loadedV2.process(args);
	const bool referenceV2Restored =
		loadedV2.engine.getEngineMode() == doorstop::EngineMode::ReferenceV2
		&& loadedV2.engine.getSpecimenSeed() == 0x2468ace0u
		&& loadedV2.engine.getReferenceV2Engine().getProfile()
			== doorstop::ReferenceSpringProfile::DarkRefinedV2;

	Doorstop referenceV3;
	referenceV3.engineMode.store(
		int(doorstop::EngineMode::ReferenceV3), std::memory_order_relaxed);
	referenceV3.specimenSeed.store(0x10293847u, std::memory_order_relaxed);
	referenceV3.referenceV3Tuning.store(
		int(doorstop::HelicalTuningVariant::DarkBoing), std::memory_order_relaxed);
	json_t* referenceV3J = referenceV3.dataToJson();
	Doorstop loadedV3;
	loadedV3.dataFromJson(referenceV3J);
	json_decref(referenceV3J);
	loadedV3.process(args);
	const bool referenceV3Restored =
		loadedV3.engine.getEngineMode() == doorstop::EngineMode::ReferenceV3
		&& loadedV3.engine.getSpecimenSeed() == 0x10293847u
		&& loadedV3.engine.getReferenceV3TuningVariant()
			== doorstop::HelicalTuningVariant::DarkBoing;
	return {"Schema migration maps legacy models and all Reference engines",
		pass && referenceRestored && referenceV2Restored && referenceV3Restored,
		"legacy=" + std::to_string(pass)
			+ " referenceV1=" + std::to_string(referenceRestored)
			+ " referenceV2=" + std::to_string(referenceV2Restored)
			+ " referenceV3=" + std::to_string(referenceV3Restored)};
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
	results.push_back(experimentalV3TuningsRoundTrip());
	results.push_back(oldPatchAndRestoreCommand());
	results.push_back(malformedBreakInJsonIsSafe());
	results.push_back(newModuleAndSpecimenSemantics());
	results.push_back(schemaMigrationMapsEveryLegacyModel());

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
