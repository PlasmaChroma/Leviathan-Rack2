#pragma once

#include "../plugin.hpp"

struct PlasmaSwitchDrawMetrics {
	uint64_t shadowNs = 0u;
	uint64_t imageEnsureNs = 0u;
	uint64_t imagePaintNs = 0u;
	uint64_t bodyNs = 0u;
	uint64_t orbNs = 0u;
	uint32_t imageCreates = 0u;
	uint32_t imageFallbacks = 0u;
	uint32_t contextResets = 0u;
};

void resetPlasmaSwitchDrawMetrics();
PlasmaSwitchDrawMetrics getPlasmaSwitchDrawMetrics();

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
	std::shared_ptr<window::Image> fallbackBackingImage;
	NVGcontext* backingImageOwnerVg = nullptr;
	int backingImageHandle = -1;
	int backingImageWidth = 0;
	int backingImageHeight = 0;
	bool backingImageCreateAttempted = false;

	PlasmaSwitch();
	~PlasmaSwitch() override;
	void step() override;
	void draw(const DrawArgs& args) override;

	void resetBackingImageHandle(NVGcontext* currentVg, bool deleteCurrentHandle);
	int ensureBackingImageHandle(NVGcontext* vg);
};
