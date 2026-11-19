#pragma once
#include "../../plugin.hpp"

struct SmallGoldApertureLight;

// Internal cache widget, owned by the light's child tree. Keeping state here
// preserves the shared light class layout and avoids rebuilding every module
// when its cache implementation changes. Never drawn automatically.
struct GoldApertureCache : widget::Widget {
	SmallGoldApertureLight* owner;
	struct CachedLayer;
	widget::FramebufferWidget* backgroundFb = nullptr;
	widget::FramebufferWidget* illuminationFb = nullptr;
	CachedLayer* backgroundLayer = nullptr;
	CachedLayer* illuminationLayer = nullptr;
	Vec cachedSize = Vec(-1.f, -1.f);
	NVGcolor cachedBaseColor = {};
	NVGcolor cachedActiveColor = {};
	float brightness = 0.f;
	float cachedBrightness = -1.f;
	float cachedBloom = -1.f;
	float cacheBleed = 0.f;
	double illuminationChangedAt = 0.;
	void syncCaches();
	void paintBackground(const DrawArgs& args);
	void paintIllumination(const DrawArgs& args);
	explicit GoldApertureCache(SmallGoldApertureLight* owner);
	void drawBackground(const DrawArgs& args);
	void drawIllumination(const DrawArgs& args);
	void onContextCreate(const ContextCreateEvent& e) override;
	void onContextDestroy(const ContextDestroyEvent& e) override;
};
