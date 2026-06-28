#pragma once

#include "../plugin.hpp"

struct PlasmaSwitch : app::Switch {
	float displayValue = 0.f;
	bool displayValueInitialized = false;
	float pulseAmount = 0.5f;
	float flickerAmount = 0.5f;
	float hueAmount = 0.5f;
	float sparkOffsetX[3] = {};
	float sparkOffsetY[3] = {};
	double lastStepSec = 0.0;
	std::string backingFullPath;
	widget::FramebufferWidget* shadowFb = nullptr;

	PlasmaSwitch();
	void step() override;
	void draw(const DrawArgs& args) override;
};
