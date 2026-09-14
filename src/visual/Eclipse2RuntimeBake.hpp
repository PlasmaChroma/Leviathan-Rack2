#pragma once
#include "../plugin.hpp"
#include <vector>

namespace eclipse2_bake {
// UI-thread, process-lifetime CPU pixels. No persistent GL objects here.
struct Raster {
	using Draw = void (*)(const Widget::DrawArgs&, void*);
	std::vector<unsigned char> rgba;
	bool attempted = false;
	unsigned builds = 0;
	bool prepare(Draw draw, void* user);
	void retryAfterContextCreate() { if (rgba.empty()) attempted = false; }
};
}
