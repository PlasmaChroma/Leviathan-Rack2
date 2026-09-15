#include "../src/plugin.hpp"
#include "../src/BifurxLicense.hpp"

#include <chrono>
#include <iostream>

#if !defined(LEVIATHAN_PRO_DRM) || !LEVIATHAN_PRO_DRM
#error "This test requires the Premium configuration and real VCV DRM header"
#endif

Plugin* pluginInstance = nullptr;
drm::Context* leviathanDrmContext = nullptr;
std::string leviathanPluginUserRootPath() { return asset::user(); }
bool isDragonKingDebugEnabled() { return false; }
bool isModuleTeardownLoggingEnabled() { return false; }
void refreshDragonKingDebugEnabled() {}
ModuleTeardownTimer::ModuleTeardownTimer(const char* name) : moduleName(name) {}
void ModuleTeardownTimer::begin(int) {}
ModuleTeardownTimer::~ModuleTeardownTimer() {}

#include "../src/Bifurx.cpp"

int main() {
	int failures = 0;
	auto check = [&](const char* name, bool pass) {
		std::cout << (pass ? "[PASS] " : "[FAIL] ") << name << '\n';
		if (!pass) ++failures;
	};
	// A private test profile, with no account token and no network requests.
	const std::string root = system::join(system::getTempDirectory(),
		"bifurx-license-spec-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
	asset::userDir = root;
	settings::token.clear();
	system::createDirectories(root);
	{
		bifurx::Bifurx module;
		Module::ProcessArgs args{};
		args.sampleRate = 48000.f;
		args.sampleTime = 1.f / args.sampleRate;
		auto seedOutput = [&]() {
			module.outputs[bifurx::Bifurx::OUT_OUTPUT].channels = 4;
			for (int c = 0; c < 4; ++c)
				module.outputs[bifurx::Bifurx::OUT_OUTPUT].setVoltage(7.f, c);
		};
		auto silent = [&]() {
			auto& output = module.outputs[bifurx::Bifurx::OUT_OUTPUT];
			bool pass = output.getChannels() == 1;
			for (int c = 0; c < 4; ++c) pass = pass && output.getVoltage(c) == 0.f;
			return pass;
		};
		seedOutput();
		module.process(args);
		check("missing DRM context clears stale output and extra channels", silent());
		drm::Context context("Leviathan-Pro");
		// Finish the no-token worker before inspecting its state in this test.
		context.queryThread.join();
		leviathanDrmContext = &context;
		check("real DRM rejects the isolated profile without a license", !bifurx::isLicenseVerified());
		check("vendored DRM uses the expected plugin license path",
			context.keyPath == system::join(root, "licenses/Leviathan-Pro.vcvkey"));
		seedOutput();
		module.params[bifurx::Bifurx::LEVEL_PARAM].setValue(1.f);
		module.inputs[bifurx::Bifurx::IN_INPUT].channels = 4;
		for (int c = 0; c < 4; ++c) module.inputs[bifurx::Bifurx::IN_INPUT].setVoltage(5.f, c);
		module.process(args);
		check("unlicensed processing stays silent with input and changed parameters", silent());
		seedOutput();
		module.processBypass(args);
		check("unlicensed bypass cannot pass through input or hold stale output", silent());
		leviathanDrmContext = nullptr;
	}
	system::removeRecursively(root);
	std::cout << "Premium licensing test: " << (failures ? "failed" : "passed") << '\n';
	return failures ? 1 : 0;
}
