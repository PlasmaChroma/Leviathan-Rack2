#pragma once

#include "../plugin.hpp"
#include "../GlResourceRetirement.hpp"

namespace visual_assets {

struct AdaptiveGlSurfacePolicy {
	float minDensity = 0.25f;
	float maxDensity = 2.f;
	int sizeQuantum = 16;
	// Number of generic vertex attributes the callback mutates (0..4).
	int vertexAttributeCount = 0;
	// Keep the largest capacity reached in this graphics context while allowing
	// the active viewport to continue following the current logical size.
	bool retainPeakCapacity = false;
	// Restricted batch contract: shader programs, attributes 0..3, texture unit 0,
	// unpack state, framebuffer bindings, enables, blend/color, viewport and polygon
	// state only. No matrices, legacy client arrays, or other compatibility state.
	bool shaderOnlyState = false;
};

class AdaptiveGlSurface {
public:
	// Callbacks may use compatibility texture units and generic attributes 0..3.
	// Set policy.vertexAttributeCount to cover the attributes they mutate.
	using RenderCallback = void (*)(void* user, Vec activeSize, int viewportY);
	// Opt-in shader passes only. Each callback establishes its program/inputs;
	// the batch supplies a documented neutral state and restores the host once.
	// Caller proves visibility/destination and presents outputs after completion.
	// Do not update images already presented in the current host recording.
	// Callbacks use unit 0 / attributes 0..3, balance matrix stacks, and establish
	// any state beyond the neutral shader baseline documented in the experiment.
	struct Update {
		AdaptiveGlSurface* surface = nullptr;
		Vec logicalSize;
		float zoom = 1.f;
		float pixelRatio = 1.f;
		AdaptiveGlSurfacePolicy policy;
		bool validate = false;
		RenderCallback callback = nullptr;
		void* user = nullptr;
		bool rendered = false;
	};
	struct BatchStats {
        size_t updates = 0, hostBoundaries = 0;
        // Optional CPU diagnostics. Only measured when a stats pointer is passed.
        uint64_t captureNs = 0, setupNs = 0, targetNs = 0, callbackNs = 0, restoreNs = 0;
    };
	// Returns false for malformed requests, duplicates, or >64 updates. Such
	// input is rejected before rendering. Per-update success is in rendered.
	// Requested capacity extents and size quantum are bounded to 16384 pixels;
	// actual allocation remains subject to the driver's limits. No GL on hits.
	static bool renderBatch(NVGcontext* vg, Update* updates, size_t count, BatchStats* stats = nullptr);

	AdaptiveGlSurface() = default;
	~AdaptiveGlSurface();
	AdaptiveGlSurface(const AdaptiveGlSurface&) = delete;
	AdaptiveGlSurface& operator=(const AdaptiveGlSurface&) = delete;

	void reset(bool deleteGlObjects);
	void markDirty() { dirty = true; }
	bool isDirty() const { return dirty; }

	bool renderIfNeeded(NVGcontext* vg,
		Vec logicalSize,
		float rackZoom,
		float windowPixelRatio,
		const AdaptiveGlSurfacePolicy& policy,
		bool validate,
		RenderCallback callback,
		void* user);

	bool draw(const Widget::DrawArgs& args, Vec logicalSize, float alpha = 1.f) const;
	// Exact pixel-density presentation for subpixel-aligned stroke surfaces.
	bool drawAligned(const Widget::DrawArgs& args, Vec logicalSize, float density, Vec pixelOffset) const;
    bool imageValid() const;

	NVGcontext* context() const { return vg; }
	int activeWidth() const { return frontActiveWidth; }
	int activeHeight() const { return frontActiveHeight; }
	int capacityWidth() const { return frontCapacityWidth; }
	int capacityHeight() const { return frontCapacityHeight; }
	uint64_t generation() const { return surfaceGeneration; }

private:
	struct BatchScope;
	bool renderImpl(NVGcontext* vg, Vec logicalSize, float zoom, float pixelRatio,
		const AdaptiveGlSurfacePolicy& policy, bool validate,
		RenderCallback callback, void* user, BatchScope* sharedScope);
	bool ensureBackSurface(NVGcontext* targetVg, int width, int height, bool validate);

	NVGLUframebuffer* front = nullptr;
	NVGLUframebuffer* back = nullptr;
	NVGcontext* vg = nullptr;
	gl_lifecycle::ContextLease resourceContext;
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
