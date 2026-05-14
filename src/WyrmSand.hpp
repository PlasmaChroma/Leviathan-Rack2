#pragma once

#include "Wyrm.hpp"

#include <array>
#include <cstdint>
#include <vector>

struct WyrmSand {
	double lastUpdateSec = -1.0;
	bool initialized = false;
	int w = 0;
	int h = 0;
	std::vector<float> depth;
	std::vector<float> energy;
	std::vector<float> baseNoise;
	int imageHandle = -1;
	int imageW = 0;
	int imageH = 0;
	int imageUploadedW = 0;
	int imageUploadedH = 0;
	bool imageDirty = true;
	uint64_t imageRevision = 1;
	uint64_t imageUploadedRevision = 0;
	std::vector<unsigned char> imagePixels;
	std::array<Vec, kWyrmPointCountMax> previousPath {};
	int previousPathCount = 0;

	void resetHistory();
	void ensureField(Vec size, int detailSetting);
	void markImageDirty();
	void ensureImageRaster(Vec size, int detailSetting);
	const unsigned char* imageData() const;
	int imageWidth() const;
	int imageHeight() const;
	uint64_t imageDataRevision() const;
	void stamp(Vec size, Vec pos, float radiusPx, float depthDelta, float energyDelta);
	void disturbSegment(Vec size, Vec a, Vec b, float troughStrength, float ridgeStrength, float energyStrength);
	void update(Vec size, double nowSec, int detailSetting, const std::array<Vec, kWyrmPointCountMax>& currentPath, int pathCount, float slitherAmount);
	void drawFlatBackground(NVGcontext* vg, Vec size) const;
	void drawNanoVGCells(NVGcontext* vg, Vec size, int detailSetting);
	void drawNanoVGImage(NVGcontext* vg, Vec size, int detailSetting);
	void draw(NVGcontext* vg, Vec size, bool enabled, int backendSetting, int detailSetting);
};
