#include "MoiraiCompiler.hpp"
#include "MoiraiCurves.hpp"
#include "MoiraiPresets.hpp"

#include <cmath>
#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
moirai::Bank bankForProgram(const moirai::Program& program) {
	moirai::Bank bank;
	bank.programs[program.id] = program;
	for (moirai::Lane& lane : bank.lanes) lane.defaultProgram = program.id;
	return bank;
}
bool hasError(const moirai::CompileResult& result, const char* code, const char* path) {
	for (const moirai::ValidationIssue& issue : result.errors)
		if (issue.code == code && issue.path == path) return true;
	return false;
}
}

int main() {
	const moirai::CompileResult initial = moirai::compileBank(moirai::makeInitialBank());
	check(initial.valid && initial.bank && initial.bank->programs.size() == 1,
		"initial authored bank compiles to one editable ADSR program");
	check(initial.valid && initial.bank->lanes[0].defaultProgram == 0 && initial.bank->lanes[1].defaultProgram == 0,
		"both initial lanes resolve their factory ADSR default");
	check(initial.valid && initial.bank->programs[0].macroBindings.size() == 1 &&
		initial.bank->programs[0].macroBindings[0].source == moirai::MacroSource::VELOCITY,
		"compiler injects the factory velocity-to-level binding");

	bool allFactoriesCompile = true;
	bool allContoursBounded = true;
	for (const moirai::Program& preset : moirai::factoryPrograms()) {
		const moirai::CompileResult compiled = moirai::compileBank(bankForProgram(preset));
		allFactoriesCompile = allFactoriesCompile && compiled.valid && compiled.bank;
		if (compiled.valid && compiled.bank && compiled.bank->programs[0].kind == moirai::ProgramKind::CONTOUR) {
			for (int index = 0; index <= 1000; ++index) {
				const float value = moirai::evaluateContour(compiled.bank->programs[0], index / 1000.f);
				allContoursBounded = allContoursBounded && std::isfinite(value) && value >= 0.f && value <= 1.f;
			}
		}
	}
	check(moirai::factoryPrograms().size() == 23 && allFactoriesCompile,
		"all 23 stable factory programs compile independently");
	check(allContoursBounded, "every factory contour remains finite and in authored bounds");

	moirai::Bank invalidId = moirai::makeInitialBank();
	moirai::Program adsr = invalidId.programs.begin()->second;
	invalidId.programs.clear();
	adsr.id = "bad id";
	invalidId.programs[adsr.id] = adsr;
	for (moirai::Lane& lane : invalidId.lanes) lane.defaultProgram = adsr.id;
	const moirai::CompileResult invalidIdResult = moirai::compileBank(invalidId);
	check(!invalidIdResult.valid && hasError(invalidIdResult, "invalid_id", "/programs/bad id"),
		"compiler rejects program ids outside the stable identifier contract");

	moirai::Bank missingAssignment = moirai::makeInitialBank();
	missingAssignment.lanes[0].assignments[7] = "missing";
	const moirai::CompileResult missingResult = moirai::compileBank(missingAssignment);
	check(!missingResult.valid && hasError(missingResult, "missing_program", "/lanes/A/assignments/7"),
		"compiler rejects sparse assignments to missing programs with a stable path");

	moirai::Bank badContour = bankForProgram(*moirai::findFactoryProgram("factory_duck"));
	badContour.programs["factory_duck"].points[2].time = badContour.programs["factory_duck"].points[1].time;
	const moirai::CompileResult contourResult = moirai::compileBank(badContour);
	check(!contourResult.valid && hasError(contourResult, "invalid_contour", "/programs/factory_duck/points/2/t"),
		"compiler rejects non-increasing contour time with a stable point path");

	moirai::Bank badLoop = moirai::makeInitialBank();
	badLoop.programs["factory_adsr"].gatePath[1].loopEnd.mode = moirai::LoopMode::COUNTED;
	badLoop.programs["factory_adsr"].gatePath[1].loopEnd.count = 4;
	const moirai::CompileResult loopResult = moirai::compileBank(badLoop);
	check(!loopResult.valid && hasError(loopResult, "invalid_loop", "/programs/factory_adsr/gatePath"),
		"compiler rejects an unmatched loop marker");

	moirai::Bank variants = moirai::makeInitialBank();
	moirai::MacroBinding binding;
	binding.target = moirai::MacroTarget::VARIANT_SELECT;
	binding.variants.push_back("missing_variant");
	variants.programs["factory_adsr"].macroBindings.push_back(binding);
	const moirai::CompileResult variantResult = moirai::compileBank(variants);
	check(!variantResult.valid && hasError(variantResult, "missing_program",
		"/programs/factory_adsr/macroBindings/0/variants/0"),
		"compiler resolves and validates variant targets away from the audio thread");

	moirai::Bank maximumPrograms = moirai::makeInitialBank();
	const moirai::Program base = maximumPrograms.programs.begin()->second;
	maximumPrograms.programs.clear();
	for (int index = 0; index < moirai::kMaxPrograms; ++index) {
		moirai::Program copy = base;
		copy.id = "p" + std::to_string(index);
		maximumPrograms.programs[copy.id] = copy;
	}
	for (moirai::Lane& lane : maximumPrograms.lanes) lane.defaultProgram = "p0";
	check(moirai::compileBank(maximumPrograms).valid, "compiler accepts the exact 32-program bank limit");
	moirai::Program overflow = base;
	overflow.id = "overflow";
	maximumPrograms.programs[overflow.id] = overflow;
	check(!moirai::compileBank(maximumPrograms).valid, "compiler rejects a 33rd authored program");

	std::cout << (failures ? "[SUMMARY] moirai_compiler_spec: FAILED\n" : "[SUMMARY] moirai_compiler_spec: passed\n");
	return failures ? 1 : 0;
}
