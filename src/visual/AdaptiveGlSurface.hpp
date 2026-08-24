#pragma once

#include "../plugin.hpp"

namespace visual_assets {

struct AdaptiveGlSurfacePolicy {
	float minDensity = 0.25f;
	float maxDensity = 2.f;
	int sizeQuantum = 16;
};

class AdaptiveGlSurface {
public:
	using RenderCallback = void (*)(void* user, Vec activeSize, int viewportY);

	AdaptiveGlSurface() = default;
	~AdaptiveGlSurface();
	AdaptiveGlSurface(const AdaptiveGlSurface&) = delete;
	AdaptiveGlSurface& operator=(const AdaptiveGlSurface&) = delete;

	void reset(bool deleteGlObjects);
	void markDirty() { dirty = true; }

	bool renderIfNeeded(NVGcontext* vg,
		Vec logicalSize,
		float rackZoom,
		float windowPixelRatio,
		const AdaptiveGlSurfacePolicy& policy,
		bool validate,
		RenderCallback callback,
		void* user);

	bool draw(const Widget::DrawArgs& args, Vec logicalSize, float alpha = 1.f) const;

	NVGcontext* context() const { return vg; }
	int activeWidth() const { return frontActiveWidth; }
	int activeHeight() const { return frontActiveHeight; }
	int capacityWidth() const { return frontCapacityWidth; }
	int capacityHeight() const { return frontCapacityHeight; }
	uint64_t generation() const { return surfaceGeneration; }

private:
	bool ensureBackSurface(NVGcontext* targetVg, int width, int height, bool validate);

	NVGLUframebuffer* front = nullptr;
	NVGLUframebuffer* back = nullptr;
	NVGcontext* vg = nullptr;
	int frontCapacityWidth = 0;
	int frontCapacityHeight = 0;
	int backCapacityWidth = 0;
	int backCapacityHeight = 0;
	int frontActiveWidth = 0;
	int frontActiveHeight = 0;
	bool dirty = true;
	uint64_t surfaceGeneration = 0;
};

} // namespace visual_assets
