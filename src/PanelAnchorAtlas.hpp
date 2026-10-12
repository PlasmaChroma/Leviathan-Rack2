#pragma once

#include "plugin.hpp"

#include <string>

namespace panel_svg {

struct PanelAnchorLookupResult {
	bool found = false;
	bool hasCenter = false;
	bool hasRect = false;
	bool hasRadius = false;
	float unitToMmScale = 0.01f;
	float cx = 0.f;
	float cy = 0.f;
	float x = 0.f;
	float y = 0.f;
	float width = 0.f;
	float height = 0.f;
	float radius = 0.f;
};

enum class PanelAnchorAtlasStatus {
	Missing,
	StaleOrUnreadable,
	Valid,
};

bool lookupPanelAnchor(const std::string& svgPath, const std::string& elementId, PanelAnchorLookupResult* out);
PanelAnchorAtlasStatus getPanelAnchorAtlasStatus(const std::string& svgPath);

} // namespace panel_svg
