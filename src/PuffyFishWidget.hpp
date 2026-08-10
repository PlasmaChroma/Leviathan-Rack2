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
	widget::FramebufferWidget* compassFramebuffer = nullptr;
	Widget* compassRasterWidget = nullptr;
	int pointerNegativeCharacter = -1;
	int pointerPositiveCharacter = -1;
	int previousPointerNegativeCharacter = -1;
	int previousPointerPositiveCharacter = -1;
	float pointerTintTransition = 1.f;

	explicit PuffyFishWidget(Puffy* module, bool roamingAvatar = false);

	void onContextDestroy(const ContextDestroyEvent& e) override;
	void step() override;
	void draw(const DrawArgs& args) override;
	void drawRoamingDropShadow(NVGcontext* vg);
	Vec visibleBodyCenter() const;
	Vec compassCenter() const;

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
	bool drawBodyShadowRaster(
		NVGcontext* vg,
		Vec center,
		float radiusX,
		float radiusY);
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
