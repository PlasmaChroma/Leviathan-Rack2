#pragma once

#include "Puffy.hpp"

#include <array>
#include <vector>

struct PuffyTransferPreviewWidget final : TransparentWidget {
	static constexpr int POINT_COUNT = 129;
	static constexpr int SWARM_COLUMN_COUNT = 65;
	static constexpr int SWARM_SAMPLES_PER_COLUMN = 3;
	static constexpr int SWARM_POINT_COUNT =
		SWARM_COLUMN_COUNT * SWARM_SAMPLES_PER_COLUMN;
	// Normalized audio domain: +/-1 is +/-5 V.
	static constexpr float DOMAIN = 1.f;

	Puffy* module = nullptr;
	PuffyVisualState visual;
	std::vector<Vec> curvePoints;
	std::array<Vec, SWARM_POINT_COUNT> swarmPoints {};
	int swarmPointCount = 0;
	widget::FramebufferWidget* curveFramebuffer = nullptr;
	TransparentWidget* curveLayer = nullptr;
	bool pointsValid = false;
	float lastAmount = -1.f;
	float lastFast = -1.f;
	float lastTransient = -1.f;
	int lastAmountBin = -1;
	int lastNegativeCharacter = -1;
	int lastPositiveCharacter = -1;
	Vec lastCurveSize;
	double lastCurveRebuildTime = -1.0;

	explicit PuffyTransferPreviewWidget(Puffy* module);

	void step() override;
	void draw(const DrawArgs& args) override;
	void drawCurve(const DrawArgs& args) const;

private:
	void rebuildPoints();
	float inputToX(float input) const;
	float outputToY(float output) const;
};
