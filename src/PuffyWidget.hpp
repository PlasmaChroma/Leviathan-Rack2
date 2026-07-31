#pragma once

#include "Puffy.hpp"

struct PuffyWidget final : ModuleWidget {
	debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;

	explicit PuffyWidget(Puffy* module);
	void step() override;
	void draw(const DrawArgs& args) override;
	void appendContextMenu(Menu* menu) override;
};
