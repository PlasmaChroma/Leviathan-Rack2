#pragma once

#include "Puffy.hpp"
#include "PuffyCharacterController.hpp"

struct PuffyFishWidget final : TransparentWidget {
	Puffy* module = nullptr;
	PuffyCharacterController controller;
	PuffyPose pose;
	PuffyVisualState visual;
	float updateAccumulator = 0.f;

	explicit PuffyFishWidget(Puffy* module);

	void step() override;
	void draw(const DrawArgs& args) override;

private:
	bool drawBodyRaster(
		NVGcontext* vg,
		Vec center,
		float radiusX,
		float radiusY,
		NVGcolor negativeTint,
		NVGcolor positiveTint) const;
	void drawFin(
		NVGcontext* vg,
		Vec center,
		float bodyRadius,
		bool left,
		float angle,
		float sizeScale,
		int imageHandle,
		int imageWidth,
		int imageHeight,
		NVGcolor tint) const;
	void drawEye(
		NVGcontext* vg,
		Vec center,
		float radius,
		float gazeX,
		float gazeY,
		float blink,
		NVGcolor eyelidColor) const;
};
