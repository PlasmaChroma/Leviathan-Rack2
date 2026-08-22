#include "OctaviaCableValidation.hpp"

#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
}

int main() {
	octavia::CableEndpointValidation endpoint{
		"input", 42, "Fundamental:ADSR", 1, 2, true, true, true};
	check(octavia::validateCableEndpoint(endpoint).empty(), "valid endpoint is accepted");

	endpoint.portId = 4;
	const std::string rangeError = octavia::validateCableEndpoint(endpoint);
	check(rangeError.find("input port 4 out of range") != std::string::npos
		&& rangeError.find("Fundamental:ADSR module 42") != std::string::npos
		&& rangeError.find("valid IDs are 0-1") != std::string::npos,
		"invalid endpoint reports direction, module, requested ID, and valid range");

	endpoint.portId = -1;
	check(octavia::validateCableEndpoint(endpoint).find("input port -1 out of range") != std::string::npos,
		"negative port ID is rejected");

	endpoint.portId = 0;
	endpoint.portWidgetExists = false;
	check(octavia::validateCableEndpoint(endpoint).find("port widget 0 not found") != std::string::npos,
		"missing Rack port widget is rejected");

	endpoint.moduleExists = false;
	check(octavia::validateCableEndpoint(endpoint) == "input module not found: 42",
		"missing module identifies the failed endpoint");

	std::cout << "[SUMMARY] octavia_cable_validation_spec: "
		<< (failures ? "FAILED" : "passed") << "\n";
	return failures ? 1 : 0;
}
