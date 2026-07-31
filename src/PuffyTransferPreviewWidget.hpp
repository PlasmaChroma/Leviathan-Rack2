#pragma once

#include "Puffy.hpp"

#include <array>

struct PuffyTransferPreviewWidget final : TransparentWidget {
	static constexpr int POINT_COUNT = 129;
	static constexpr float DOMAIN = 1.25f;

	Puffy* module = nullptr;
	PuffyVisualState visual;
	std::array<Vec, POINT_COUNT> points {};
	widget::FramebufferWidget* curveFramebuffer = nullptr;
	TransparentWidget* curveLayer = nullptr;
	bool pointsValid = false;
	float lastAmount = -1.f;
	float lastFast = -1.f;
	float lastTransient = -1.f;
	int lastCharacter = -1;
	Vec lastCurveSize;

	explicit PuffyTransferPreviewWidget(Puffy* module);

	void step() override;
	void draw(const DrawArgs& args) override;
	void drawCurve(const DrawArgs& args) const;

private:
	void rebuildPoints();
	float inputToX(float input) const;
	float outputToY(float output) const;
};
