#include "OctaviaActionValidation.hpp"

#include <iostream>
#include <limits>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
}

int main() {
	check(octavia::validateRackPosition(2092.f, 100).empty(), "large real patch coordinates remain valid");
	check(!octavia::validateRackPosition(std::numeric_limits<float>::infinity(), 0).empty(),
		"infinite HP is rejected");
	check(!octavia::validateRackPosition(0.f, octavia::kMaxAbsRackRow + 1).empty(),
		"extreme row is rejected");
	check(octavia::validateParameterValue(0.5f).empty(), "finite parameter value is valid");
	check(!octavia::validateParameterValue(std::numeric_limits<float>::quiet_NaN()).empty(),
		"NaN parameter value is rejected");
	std::cout << "[SUMMARY] octavia_action_validation_spec: "
		<< (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
