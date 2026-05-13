#pragma once

#include "Wyrm.hpp"

#include <array>
#include <vector>

struct WyrmSand {
	double lastUpdateSec = -1.0;
	bool initialized = false;
	int w = 0;
	int h = 0;
	std::vector<float> depth;
	std::vector<float> energy;
	std::vector<float> baseNoise;
	std::array<Vec, kWyrmPointCountMax> previousPath {};
	int previousPathCount = 0;

	void resetHistory();
	void ensureField(Vec size);
	void stamp(Vec size, Vec pos, float radiusPx, float depthDelta, float energyDelta);
	void disturbSegment(Vec size, Vec a, Vec b, float troughStrength, float ridgeStrength, float energyStrength);
	void update(Vec size, double nowSec, const std::array<Vec, kWyrmPointCountMax>& currentPath, int pathCount, float slitherAmount);
	void drawFlatBackground(NVGcontext* vg, Vec size) const;
	void drawCells(NVGcontext* vg, Vec size);
	void draw(NVGcontext* vg, Vec size, bool enabled);
};
