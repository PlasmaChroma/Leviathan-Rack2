#include "../src/MandelwakeEngine.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

struct TestContext {
	int checks = 0;
	int failures = 0;

	void expect(bool condition, const std::string& name) {
		++checks;
		if (condition) return;
		++failures;
		std::cerr << "[FAIL] " << name << '\n';
	}
};

mandelwake::OrbitQ28 q28(double value) {
	return static_cast<mandelwake::OrbitQ28>(
		std::llround(value * static_cast<double>(mandelwake::kScaleQ28)));
}

mandelwake::StepInputs defaultInputs() {
	mandelwake::StepInputs inputs;
	inputs.cXQ28 = q28(-0.75);
	inputs.cYQ28 = 0;
	inputs.mutationDepthQ28 = q28(0.015);
	inputs.densityQ16 = 32768;
	inputs.iterations = 4;
	return inputs;
}

void tableContract(TestContext& test) {
	test.expect(mandelwake::kZoomTableSize == 3073, "ZOOM table size");
	test.expect(mandelwake::kRateTableSize == 3065, "RATE table size");
	test.expect(mandelwake::kPhaseAtanTableSize == 4097, "phase table size");
	test.expect(mandelwake::kZoomScaleQ28.front() == (1 << 28), "ZOOM table starts at unity");
	test.expect(mandelwake::kZoomScaleQ28.back() == (1 << 16), "ZOOM table ends at 12 octaves");
	test.expect(mandelwake::kRateMicroHz.front() == 50000u, "RATE table low clamp");
	test.expect(mandelwake::kRateMicroHz.back() == 200000000u, "RATE table high clamp");
	test.expect(mandelwake::kPhaseAtanQ30.front() == 0, "phase table starts at zero");
	test.expect(mandelwake::kPhaseAtanQ30.back() == (1 << 28), "phase table ends at pi/4");
	test.expect(std::strcmp(
		mandelwake::kZoomTableSha256,
		"47a19469549d0956f00ca5e3663c5f8e1a3e5106b7eddf0a1a79d7c15c158868") == 0,
		"ZOOM table checksum");
	test.expect(std::strcmp(
		mandelwake::kRateTableSha256,
		"99743d844a978732e45abccb2c2df12fac83df5e22419031227cf6ebcd2e6e2d") == 0,
		"RATE table checksum");
	test.expect(std::strcmp(
		mandelwake::kPhaseAtanTableSha256,
		"60d67eaa6614bb7972a09df2d6d53ef6779d09ff6947cb2f5c0b3145915eb662") == 0,
		"phase table checksum");
}

void fixedPointContract(TestContext& test) {
	test.expect(mandelwake::divideRoundHalfAway(1, 2) == 1, "positive half rounds away");
	test.expect(mandelwake::divideRoundHalfAway(-1, 2) == -1, "negative half rounds away");
	test.expect(mandelwake::divideRoundHalfAway(1, 3) == 0, "positive below half rounds down");
	test.expect(mandelwake::divideRoundHalfAway(-1, 3) == 0, "negative below half rounds to zero");
	test.expect(mandelwake::divideRoundHalfAway(5, 2) == 3, "positive two-and-half rounds away");
	test.expect(mandelwake::divideRoundHalfAway(-5, 2) == -3, "negative two-and-half rounds away");

	test.expect(mandelwake::mix64(0) == UINT64_C(0xE220A8397B1DCDAF), "SplitMix64 zero vector");
	test.expect(
		mandelwake::mix64(UINT64_C(0x123456789ABCDEF0)) == UINT64_C(0x161922C645CE50E8),
		"SplitMix64 nonzero vector");
	test.expect(
		mandelwake::orbitHash(
			UINT64_C(0x0123456789ABCDEF), mandelwake::kDomainGate,
			3u, 2u, 42u, 0u) == UINT64_C(0x20C3FB75C79824BE),
		"orbit hash field order vector");

	test.expect(mandelwake::integerSqrt64(0) == 0u, "sqrt zero");
	test.expect(mandelwake::integerSqrt64(1) == 1u, "sqrt one");
	test.expect(mandelwake::integerSqrt64(2) == 1u, "sqrt floors two");
	test.expect(mandelwake::integerSqrt64(4) == 2u, "sqrt four");
	test.expect(
		mandelwake::integerSqrt64(std::numeric_limits<std::uint64_t>::max())
			== std::numeric_limits<std::uint32_t>::max(),
		"sqrt uint64 maximum");
	test.expect(mandelwake::radiusQ28(q28(3.0), q28(4.0)) == static_cast<std::uint32_t>(q28(5.0)),
		"Q4.28 radius 3-4-5 vector");

	test.expect(mandelwake::phaseQ30(0, 0) == 0, "phase zero vector");
	test.expect(mandelwake::phaseQ30(q28(1.0), 0) == 0, "phase positive X axis");
	test.expect(mandelwake::phaseQ30(0, q28(1.0)) == (1 << 29), "phase positive Y axis");
	test.expect(mandelwake::phaseQ30(q28(-1.0), 0) == (1 << 30), "phase negative X axis");
	test.expect(mandelwake::phaseQ30(0, q28(-1.0)) == -(1 << 29), "phase negative Y axis");
	test.expect(mandelwake::phaseQ30(q28(1.0), q28(1.0)) == (1 << 28), "phase first diagonal");
	test.expect(mandelwake::phaseQ30(q28(-1.0), q28(-1.0)) == -(3 << 28),
		"phase third diagonal");
}

struct GoldenStep {
	std::int32_t xQ28;
	std::int32_t yQ28;
	std::uint32_t radiusQ28;
	std::int32_t phaseQ30;
	bool gate;
	bool escaped;
};

std::vector<GoldenStep> renderDefaultTrace() {
	mandelwake::Engine engine(UINT64_C(0x0123456789ABCDEF));
	const mandelwake::StepInputs inputs = defaultInputs();
	std::vector<GoldenStep> trace;
	for (int i = 0; i < 8; ++i) {
		const mandelwake::StepOutputs output = engine.step(0, inputs);
		trace.push_back({
			output.xQ28, output.yQ28, output.radiusQ28, output.phaseQ30,
			output.gate, output.escaped});
	}
	return trace;
}

bool sameGoldenStep(const GoldenStep& a, const GoldenStep& b) {
	return a.xQ28 == b.xQ28
		&& a.yQ28 == b.yQ28
		&& a.radiusQ28 == b.radiusQ28
		&& a.phaseQ30 == b.phaseQ30
		&& a.gate == b.gate
		&& a.escaped == b.escaped;
}

void engineContract(TestContext& test) {
	const std::array<GoldenStep, 8> expected {{
		{-65295784, 2808897, 65356172u, 1059064880, true, false},
		{-77860129, 2445273, 77898517u, 1063064591, false, false},
		{-79846340, 1308087, 79857054u, 1068151640, false, false},
		{-87799266, 4865991, 87934003u, 1054819613, false, false},
		{-91860446, 8949487, 92295367u, 1040552770, false, false},
		{-91585854, -4519504, 91697298u, -1056899977, false, false},
		{-91399088, 116121, 91399161u, 1073324609, false, false},
		{-94160425, 1457653, 94171706u, 1068485328, true, false},
	}};
	const std::vector<GoldenStep> first = renderDefaultTrace();
	const std::vector<GoldenStep> second = renderDefaultTrace();
	test.expect(first.size() == second.size(), "repeat trace size");
	for (std::size_t i = 0; i < first.size(); ++i) {
		test.expect(sameGoldenStep(first[i], second[i]),
			"repeat trace step " + std::to_string(i));
		test.expect(sameGoldenStep(first[i], expected[i]),
			"default golden trace step " + std::to_string(i));
	}

	mandelwake::Engine resetEngine(UINT64_C(0x0123456789ABCDEF));
	const mandelwake::StepInputs inputs = defaultInputs();
	const mandelwake::StepOutputs beforeReset = resetEngine.step(0, inputs);
	for (int i = 0; i < 7; ++i) resetEngine.step(0, inputs);
	resetEngine.resetChannel(0);
	const mandelwake::StepOutputs afterReset = resetEngine.step(0, inputs);
	test.expect(beforeReset.xQ28 == afterReset.xQ28 && beforeReset.yQ28 == afterReset.yQ28,
		"RESET repeats first orbit point");
	test.expect(beforeReset.gate == afterReset.gate && beforeReset.escaped == afterReset.escaped,
		"RESET repeats first events");
	test.expect(afterReset.stepIndex == 0u, "first accepted step uses index zero");

	mandelwake::Engine mapEngine(UINT64_C(0x0123456789ABCDEF));
	const mandelwake::StepOutputs mandelbrot = mapEngine.step(0, inputs);
	mapEngine.setMap(mandelwake::Map::Julia);
	const mandelwake::StepOutputs julia = mapEngine.step(0, inputs);
	mapEngine.setMap(mandelwake::Map::BurningShip);
	const mandelwake::StepOutputs ship = mapEngine.step(0, inputs);
	test.expect(
		mandelbrot.xQ28 != julia.xQ28 || mandelbrot.yQ28 != julia.yQ28,
		"Julia trace differs from Mandelbrot");
	test.expect(
		ship.xQ28 != julia.xQ28 || ship.yQ28 != julia.yQ28,
		"Burning Ship trace differs from Julia");

	mandelwake::Engine escapeEngine(UINT64_C(0x9988776655443322));
	mandelwake::StepInputs escapeInputs;
	escapeInputs.cXQ28 = q28(1.0);
	escapeInputs.cYQ28 = 0;
	escapeInputs.iterations = 8;
	escapeInputs.densityQ16 = 0;
	const mandelwake::StepOutputs escaped = escapeEngine.step(0, escapeInputs);
	test.expect(escaped.escaped, "escaping orbit emits ESCAPE");
	test.expect(!escaped.gate, "zero density never emits GATE");
	test.expect(escaped.xQ28 >= q28(-0.125) && escaped.xQ28 < q28(0.125),
		"re-entry X is bounded");
	test.expect(escaped.yQ28 >= q28(-0.125) && escaped.yQ28 < q28(0.125),
		"re-entry Y is bounded");

	mandelwake::Engine historyEngine(UINT64_C(0x13579BDF2468ACE0));
	for (int i = 0; i < 300; ++i) historyEngine.step(0, inputs);
	test.expect(historyEngine.channel(0).historyCount == mandelwake::kHistoryCapacity,
		"history ring remains bounded");
	const mandelwake::HistoryPoint newest = historyEngine.historyPointOldestFirst(
		0, mandelwake::kHistoryCapacity - 1);
	test.expect(
		newest.xQ28 == historyEngine.channel(0).xQ28
			&& newest.yQ28 == historyEngine.channel(0).yQ28,
		"history newest point matches channel state");

	mandelwake::Engine polyEngine(UINT64_C(0xCAFEBABEDEADBEEF));
	polyEngine.setMap(mandelwake::Map::Julia);
	const mandelwake::HistoryPoint channel0 = polyEngine.historyPointOldestFirst(0, 0);
	const mandelwake::HistoryPoint channel15 = polyEngine.historyPointOldestFirst(15, 0);
	test.expect(channel0.xQ28 != channel15.xQ28 || channel0.yQ28 != channel15.yQ28,
		"Julia seed separates channels");

	const std::uint64_t oldSeed = polyEngine.baseSeed();
	const std::uint64_t newSeed = polyEngine.reseed();
	test.expect(newSeed != oldSeed, "RESEED changes base seed");
	test.expect(polyEngine.channel(0).stepIndex == 0u, "RESEED resets step index");
}

void printGoldenTrace() {
	const std::vector<GoldenStep> trace = renderDefaultTrace();
	for (const GoldenStep& step : trace) {
		std::cout << "{" << step.xQ28 << ", " << step.yQ28 << ", "
			<< step.radiusQ28 << "u, " << step.phaseQ30 << ", "
			<< (step.gate ? "true" : "false") << ", "
			<< (step.escaped ? "true" : "false") << "},\n";
	}
}

} // namespace

int main(int argc, char** argv) {
	if (argc > 1 && std::string(argv[1]) == "--print-golden") {
		printGoldenTrace();
		return 0;
	}

	TestContext test;
	tableContract(test);
	fixedPointContract(test);
	engineContract(test);
	if (test.failures != 0) {
		std::cerr << "Mandelwake engine spec: " << test.failures << " of "
			<< test.checks << " checks failed\n";
		return 1;
	}
	std::cout << "Mandelwake engine spec: " << test.checks << " checks passed\n";
	return 0;
}
