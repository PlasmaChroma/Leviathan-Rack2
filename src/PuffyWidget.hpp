#pragma once

#include "Puffy.hpp"

#include <cstdint>
#include <fstream>
#include <string>

struct PuffyRoamingOverlay;

struct PuffyWidget final : ModuleWidget {
	PuffyRoamingOverlay* roamingOverlay = nullptr;
	debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;
	std::ofstream drawLogFile;
	std::string drawLogPath;
	bool drawLogActive = false;
	std::uint64_t drawLogRowCounter = 0u;

	explicit PuffyWidget(Puffy* module);
	~PuffyWidget() override;
	void step() override;
	void draw(const DrawArgs& args) override;
	void appendContextMenu(Menu* menu) override;

private:
	static std::string drawLogRootPath();
	static std::string drawLogDateTimeStamp();
	void stopDrawLog();
	void syncDrawLog(bool enabled, std::uint32_t instanceId);
};
