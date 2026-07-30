#pragma once

#include "Puffy.hpp"

struct PuffyWidget final : ModuleWidget {
	explicit PuffyWidget(Puffy* module);
	void appendContextMenu(Menu* menu) override;
};
