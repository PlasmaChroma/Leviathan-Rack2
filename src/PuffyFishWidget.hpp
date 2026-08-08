#pragma once

#include "Puffy.hpp"
#include "PuffyCharacterController.hpp"

struct PuffyFishWidget final : TransparentWidget {
	Puffy* module = nullptr;
	PuffyCharacterController controller;
	PuffyPose pose;
	PuffyVisualState visual;
	float updateAccumulator = 0.f;
	int bodyStableDraws = 0;
	bool transitionAtlasReady = false;
	bool roamingAvatar = false;

	explicit PuffyFishWidget(Puffy* module, bool roamingAvatar = false);

	void step() override;
	void draw(const DrawArgs& args) override;
	void drawRoamingDropShadow(NVGcontext* vg);

private:
	bool bodyTintIsSettled(
		NVGcolor negativeTint,
		NVGcolor positiveTint) const;
	bool drawTransitionBodyRaster(
		NVGcontext* vg,
		Vec center,
		float radiusX,
		float radiusY,
		NVGcolor negativeTint,
		NVGcolor positiveTint);
	bool drawBodyRaster(
		NVGcontext* vg,
		Vec center,
		float radiusX,
		float radiusY,
		NVGcolor negativeTint,
		NVGcolor positiveTint);
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
		int eyeballImageHandle,
		float gazeX,
		float gazeY,
		float blink,
		NVGcolor eyelidColor) const;
};
