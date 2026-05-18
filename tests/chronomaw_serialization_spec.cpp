#include "../src/Chronomaw.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

Plugin* pluginInstance = nullptr;

namespace {

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

bool nearlyEqual(float a, float b, float eps = 1e-6f) {
	return std::fabs(a - b) <= eps;
}

chronomaw::OutputState makeOutputState(float base, uint32_t seedBase) {
	chronomaw::OutputState out;
	out.muted = std::fmod(base, 2.f) > 0.5f;
	out.levelPct = 10.f + base;
	out.offsetPct = -50.f + base;
	out.phasePct = -25.f + base;
	out.probabilityPct = 30.f + base;
	out.invert = std::fmod(base, 3.f) > 1.2f;
	out.randomSeed = seedBase + uint32_t(base * 17.f);
	return out;
}

bool outputStateEqual(const chronomaw::OutputState& a, const chronomaw::OutputState& b) {
	return a.muted == b.muted &&
		nearlyEqual(a.levelPct, b.levelPct) &&
		nearlyEqual(a.offsetPct, b.offsetPct) &&
		nearlyEqual(a.phasePct, b.phasePct) &&
		nearlyEqual(a.probabilityPct, b.probabilityPct) &&
		a.invert == b.invert &&
		a.randomSeed == b.randomSeed;
}

TestResult testRoundTripLiveBanksUi() {
	Chronomaw src;
	src.onReset();

	src.state.live.bpm = 176.5f;
	src.state.live.running = true;
	src.state.live.activeBank = 13;
	src.state.live.density = chronomaw::DensityMode::Focus;
	src.state.ui.selectedOutput = 5;
	src.state.ui.selectedTab = 2;
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		src.state.live.outputs[size_t(i)] = makeOutputState(float(i), 100u);
	}

	const int bankA = 7;
	const int bankB = 29;
	src.state.banks[size_t(bankA)].bpm = 101.25f;
	src.state.banks[size_t(bankB)].bpm = 259.75f;
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		src.state.banks[size_t(bankA)].outputs[size_t(i)] = makeOutputState(float(i + 30), 5000u);
		src.state.banks[size_t(bankB)].outputs[size_t(i)] = makeOutputState(float(i + 60), 8000u);
	}

	json_t* root = src.dataToJson();
	if (!root) {
		return {"Chronomaw serialization round-trip (live/banks/ui)", false, "dataToJson returned null"};
	}

	Chronomaw dst;
	dst.onReset();
	dst.dataFromJson(root);
	json_decref(root);

	bool pass = true;
	pass = pass && nearlyEqual(dst.state.live.bpm, src.state.live.bpm);
	pass = pass && (dst.state.live.running == src.state.live.running);
	pass = pass && (dst.state.live.activeBank == src.state.live.activeBank);
	pass = pass && (dst.state.live.density == src.state.live.density);
	pass = pass && (dst.state.ui.selectedOutput == src.state.ui.selectedOutput);
	pass = pass && (dst.state.ui.selectedTab == src.state.ui.selectedTab);
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		pass = pass && outputStateEqual(dst.state.live.outputs[size_t(i)], src.state.live.outputs[size_t(i)]);
		pass = pass && outputStateEqual(dst.state.banks[size_t(bankA)].outputs[size_t(i)], src.state.banks[size_t(bankA)].outputs[size_t(i)]);
		pass = pass && outputStateEqual(dst.state.banks[size_t(bankB)].outputs[size_t(i)], src.state.banks[size_t(bankB)].outputs[size_t(i)]);
	}
	pass = pass && nearlyEqual(dst.state.banks[size_t(bankA)].bpm, src.state.banks[size_t(bankA)].bpm);
	pass = pass && nearlyEqual(dst.state.banks[size_t(bankB)].bpm, src.state.banks[size_t(bankB)].bpm);

	return {
		"Chronomaw serialization round-trip (live/banks/ui)",
		pass,
		"liveBpm=" + std::to_string(dst.state.live.bpm) +
		" activeBank=" + std::to_string(dst.state.live.activeBank) +
		" uiOut=" + std::to_string(dst.state.ui.selectedOutput) +
		" uiTab=" + std::to_string(dst.state.ui.selectedTab)
	};
}

TestResult testClampAndLegacyUiFallback() {
	Chronomaw dst;
	dst.onReset();

	json_t* root = json_object();
	json_object_set_new(root, "schemaVersion", json_integer(1));

	json_t* live = json_object();
	json_object_set_new(live, "bpm", json_real(9999.0));
	json_object_set_new(live, "activeBank", json_integer(999));
	json_object_set_new(live, "selectedOutput", json_integer(99)); // legacy placement
	json_object_set_new(live, "selectedTab", json_integer(4));      // legacy placement
	json_object_set_new(live, "density", json_integer(99));

	json_t* liveOutputs = json_array();
	json_t* out0 = json_object();
	json_object_set_new(out0, "levelPct", json_real(999.0));
	json_object_set_new(out0, "offsetPct", json_real(-999.0));
	json_object_set_new(out0, "phasePct", json_real(999.0));
	json_object_set_new(out0, "probabilityPct", json_real(-50.0));
	json_object_set_new(out0, "randomSeed", json_integer(-1));
	json_array_append_new(liveOutputs, out0);
	json_object_set_new(live, "outputs", liveOutputs);
	json_object_set_new(root, "live", live);

	json_t* banks = json_array();
	json_t* bank0 = json_object();
	json_object_set_new(bank0, "bpm", json_real(-1000.0));
	json_t* bankOuts = json_array();
	json_t* bankOut0 = json_object();
	json_object_set_new(bankOut0, "probabilityPct", json_real(300.0));
	json_array_append_new(bankOuts, bankOut0);
	json_object_set_new(bank0, "outputs", bankOuts);
	json_array_append_new(banks, bank0);
	json_object_set_new(root, "banks", banks);

	dst.dataFromJson(root);
	json_decref(root);

	const bool bpmClamped = nearlyEqual(dst.state.live.bpm, chronomaw::kMaxBpm);
	const bool bankClamped = dst.state.live.activeBank == (chronomaw::kNumBanks - 1);
	const bool densityClamped = dst.state.live.density == chronomaw::DensityMode::Focus;
	const bool uiLegacyRead = dst.state.ui.selectedOutput == (chronomaw::kNumOutputs - 1) && dst.state.ui.selectedTab == 4;
	const bool outClamp = nearlyEqual(dst.state.live.outputs[0].levelPct, 100.f) &&
		nearlyEqual(dst.state.live.outputs[0].offsetPct, -100.f) &&
		nearlyEqual(dst.state.live.outputs[0].phasePct, 100.f) &&
		nearlyEqual(dst.state.live.outputs[0].probabilityPct, 0.f) &&
		dst.state.live.outputs[0].randomSeed == 0u;
	const bool bankOutClamp = nearlyEqual(dst.state.banks[0].bpm, chronomaw::kMinBpm) &&
		nearlyEqual(dst.state.banks[0].outputs[0].probabilityPct, 100.f);

	const bool pass = bpmClamped && bankClamped && densityClamped && uiLegacyRead && outClamp && bankOutClamp;
	return {
		"Chronomaw deserialization clamps and legacy-ui fallback",
		pass,
		"bpm=" + std::to_string(dst.state.live.bpm) +
		" bank=" + std::to_string(dst.state.live.activeBank) +
		" uiOut=" + std::to_string(dst.state.ui.selectedOutput) +
		" uiTab=" + std::to_string(dst.state.ui.selectedTab)
	};
}

} // namespace

int main() {
	const std::vector<TestResult> tests = {
		testRoundTripLiveBanksUi(),
		testClampAndLegacyUiFallback(),
	};

	int fails = 0;
	for (const TestResult& t : tests) {
		std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name;
		if (!t.detail.empty()) {
			std::cout << " :: " << t.detail;
		}
		std::cout << "\n";
		if (!t.pass) {
			fails++;
		}
	}
	if (fails > 0) {
		std::cout << "[SUMMARY] chronomaw_serialization_spec failed " << fails << " / " << tests.size() << " tests\n";
		return 1;
	}
	std::cout << "[SUMMARY] chronomaw_serialization_spec passed " << tests.size() << " tests\n";
	return 0;
}

