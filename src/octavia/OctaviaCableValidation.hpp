#pragma once

#include <cstdint>
#include <string>

namespace octavia {

struct CableEndpointValidation {
	const char* direction = "port";
	int64_t moduleId = -1;
	std::string moduleName;
	int64_t portId = -1;
	std::size_t portCount = 0;
	bool moduleExists = false;
	bool widgetExists = false;
	bool portWidgetExists = false;

	CableEndpointValidation(const char* direction, int64_t moduleId, std::string moduleName,
		int64_t portId, std::size_t portCount, bool moduleExists, bool widgetExists,
		bool portWidgetExists)
		: direction(direction), moduleId(moduleId), moduleName(std::move(moduleName)),
		  portId(portId), portCount(portCount), moduleExists(moduleExists),
		  widgetExists(widgetExists), portWidgetExists(portWidgetExists) {}
};

inline std::string validateCableEndpoint(const CableEndpointValidation& endpoint) {
	const std::string identity = endpoint.moduleName.empty()
		? "module " + std::to_string(endpoint.moduleId)
		: endpoint.moduleName + " module " + std::to_string(endpoint.moduleId);
	if (!endpoint.moduleExists)
		return std::string(endpoint.direction) + " module not found: " + std::to_string(endpoint.moduleId);
	if (!endpoint.widgetExists)
		return std::string(endpoint.direction) + " module widget not found for " + identity;
	if (endpoint.portId < 0 || static_cast<std::size_t>(endpoint.portId) >= endpoint.portCount) {
		std::string error = std::string(endpoint.direction) + " port " + std::to_string(endpoint.portId)
			+ " out of range for " + identity + "; ";
		if (endpoint.portCount == 0) error += "module has no " + std::string(endpoint.direction) + " ports";
		else error += "valid IDs are 0-" + std::to_string(endpoint.portCount - 1);
		return error;
	}
	if (!endpoint.portWidgetExists)
		return std::string(endpoint.direction) + " port widget " + std::to_string(endpoint.portId)
			+ " not found for " + identity;
	return {};
}

} // namespace octavia
