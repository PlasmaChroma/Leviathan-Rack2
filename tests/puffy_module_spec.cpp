#include "../src/Puffy.hpp"

#include <cmath>
#include <iostream>
#include <string>

ModuleTeardownTimer::ModuleTeardownTimer(const char* moduleName)
	: moduleName(moduleName) {
}

void ModuleTeardownTimer::begin(int moduleId) {
	this->moduleId = moduleId;
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
}

bool isDragonKingDebugEnabled() {
	return false;
}

namespace {

void processBypass(Puffy& module, int64_t frame) {
	Module::ProcessArgs args;
	args.sampleRate = 48000.f;
	args.sampleTime = 1.f / args.sampleRate;
	args.frame = frame;
	module.processBypass(args);
}

void pressLimiterButton(Puffy& module, int64_t& frame) {
	module.params[Puffy::LIMITER_BUTTON_PARAM].setValue(0.f);
	processBypass(module, frame++);
	module.params[Puffy::LIMITER_BUTTON_PARAM].setValue(1.f);
	processBypass(module, frame++);
	module.params[Puffy::LIMITER_BUTTON_PARAM].setValue(0.f);
	processBypass(module, frame++);
}

bool near(float actual, float expected, float tolerance = 1e-6f) {
	return std::fabs(actual - expected) <= tolerance;
}

} // namespace

int main() {
	Puffy module;
	int64_t frame = 0;
	// In Rack, cable bookkeeping owns the connected/disconnected transition.
	// Set the unstable field directly here to emulate two connected cables.
	module.inputs[Puffy::INPUT_L].channels = 1;
	module.inputs[Puffy::INPUT_R].channels = 1;
	module.outputs[Puffy::OUTPUT_L].channels = 1;
	module.outputs[Puffy::OUTPUT_R].channels = 1;
	module.inputs[Puffy::INPUT_L].setVoltage(3.25f);
	module.inputs[Puffy::INPUT_R].setVoltage(-1.75f);

	pressLimiterButton(module, frame);
	module.params[Puffy::ROAMING_BUTTON_PARAM].setValue(1.f);
	processBypass(module, frame++);

	json_t* saved = module.dataToJson();
	json_t* savedLimiter = json_object_get(saved, "limiterMode");
	json_t* savedRoaming = json_object_get(saved, "roamingEnabled");
	const bool routingPreserved =
		module.outputs[Puffy::OUTPUT_L].getChannels() == 1
		&& module.outputs[Puffy::OUTPUT_R].getChannels() == 1
		&& near(module.outputs[Puffy::OUTPUT_L].getVoltage(), 3.25f)
		&& near(module.outputs[Puffy::OUTPUT_R].getVoltage(), -1.75f);
	const bool stateUpdated =
		module.limiterMode.load(std::memory_order_relaxed)
			== int(puffy::LimiterMode::Soft)
		&& module.roamingEnabled.load(std::memory_order_relaxed)
		&& json_is_string(savedLimiter)
		&& std::string(json_string_value(savedLimiter)) == "soft"
		&& json_is_true(savedRoaming);
	const bool lightsUpdated =
		near(module.lights[Puffy::LIMITER_HARD_LIGHT].getBrightness(), 0.f)
		&& near(module.lights[Puffy::LIMITER_SOFT_LIGHT].getBrightness(), 1.f)
		&& near(module.lights[Puffy::LIMITER_OFF_LIGHT].getBrightness(), 0.f)
		&& near(module.lights[Puffy::ROAMING_LIGHT].getBrightness(), 1.f);
	Puffy loaded;
	loaded.dataFromJson(saved);
	processBypass(loaded, 0);
	const bool stateReloaded =
		loaded.limiterMode.load(std::memory_order_relaxed)
			== int(puffy::LimiterMode::Soft)
		&& loaded.roamingEnabled.load(std::memory_order_relaxed)
		&& near(loaded.lights[Puffy::LIMITER_SOFT_LIGHT].getBrightness(), 1.f)
		&& near(loaded.lights[Puffy::ROAMING_LIGHT].getBrightness(), 1.f);
	json_decref(saved);

	module.params[Puffy::ROAMING_BUTTON_PARAM].setValue(0.f);
	processBypass(module, frame++);
	const bool roamingTurnsOff =
		!module.roamingEnabled.load(std::memory_order_relaxed)
		&& near(module.lights[Puffy::ROAMING_LIGHT].getBrightness(), 0.f);

	const bool pass = routingPreserved && stateUpdated && stateReloaded
		&& lightsUpdated && roamingTurnsOff;
	std::cout << (pass ? "[PASS] " : "[FAIL] ")
		<< "Bypass preserves dry routing while limiter and roaming controls remain live"
		<< " :: routing=" << routingPreserved
		<< " state=" << stateUpdated
		<< " reload=" << stateReloaded
		<< " lights=" << lightsUpdated
		<< " roamingOff=" << roamingTurnsOff
		<< " outputs=" << int(module.outputs[Puffy::OUTPUT_L].getChannels())
		<< "/" << module.outputs[Puffy::OUTPUT_L].getVoltage()
		<< "," << int(module.outputs[Puffy::OUTPUT_R].getChannels())
		<< "/" << module.outputs[Puffy::OUTPUT_R].getVoltage() << '\n';
	std::cout << "[SUMMARY] puffy_module_spec: " << (pass ? "1/1" : "0/1")
		<< " passed\n";
	return pass ? 0 : 1;
}
