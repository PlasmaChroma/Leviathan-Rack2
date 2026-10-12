#pragma once

#include "plugin.hpp"

namespace puffy_body_cache {

struct ImageAccess {
	int handle = -1;
	int width = 0;
	int height = 0;
	bool cacheHit = false;
	bool created = false;
	bool recolored = false;
	bool contextReset = false;
	std::uint64_t recolorNs = 0u;
	std::uint64_t uploadNs = 0u;

	explicit operator bool() const {
		return handle >= 0 && width > 0 && height > 0;
	}
};

ImageAccess ensureFinalBody(
	NVGcontext* vg,
	int negativeCharacter,
	int positiveCharacter);
ImageAccess ensureFinalPointer(
	NVGcontext* vg,
	int negativeCharacter,
	int positiveCharacter);
ImageAccess ensureTransitionAtlas(NVGcontext* vg);

// Called from Rack's ContextDestroyEvent, before NanoVG is destroyed. Handles
// are abandoned rather than deleted because the context owns their storage and
// is about to release it as a unit.
void onContextDestroy(NVGcontext* vg);
void onContextCreate(NVGcontext* vg);

} // namespace puffy_body_cache
