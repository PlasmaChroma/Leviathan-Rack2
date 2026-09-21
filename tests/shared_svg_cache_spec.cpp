#include "../src/visual/SharedSvgCacheState.hpp"
#include <cstdio>
#include <cstdlib>

struct NVGcontext { int id; };
namespace rack { namespace window { struct Svg {}; } }

static void require(bool condition, const char* message) {
	if (!condition) {
		std::fprintf(stderr, "FAIL: %s\n", message);
		std::abort();
	}
}

int main() {
	using namespace visual_assets::shared_svg_cache;
	CacheState cache;
	rack::window::Svg svg;
	NVGcontext main{1}, framebuffer{2}, other{3};
	cache.findOrCreate(&svg, &main, &main).imageHandle = 11;
	cache.findOrCreate(&svg, &framebuffer, &main).imageHandle = 22;
	cache.findOrCreate(&svg, &other, &other).imageHandle = 33;
	for (int i = 0; i < 100; ++i) {
		require(cache.findOrCreate(&svg, &main, &main).imageHandle == 11,
			"browser rendering preserves main-context image");
		require(cache.findOrCreate(&svg, &framebuffer, &main).imageHandle == 22,
			"main rendering preserves framebuffer-context image");
	}
	cache.onContextDestroy(nullptr);
	require(cache.findOrCreate(&svg, &main, &main).imageHandle == 11, "null destroy is harmless");
	cache.onContextDestroy(&main);
	cache.onContextDestroy(&main); // Many widgets receive the same event.
	require(cache.findOrCreate(&svg, &other, &other).imageHandle == 33, "unrelated context survives");
	// Reuse exact SVG and context addresses, as can happen on window reopen.
	auto& reopened = cache.findOrCreate(&svg, &main, &main);
	require(reopened.imageHandle == -1 && reopened.ownerVg == nullptr && reopened.cachedWidth == 0,
		"reopened main context starts without old image metadata");
	require(cache.findOrCreate(&svg, &framebuffer, &main).imageHandle == -1,
		"main destroy also invalidates framebuffer images");
	cache.findOrCreate(&svg, &framebuffer, &main).imageHandle = 44;
	cache.onContextDestroy(&framebuffer);
	require(cache.findOrCreate(&svg, &framebuffer, &main).imageHandle == -1,
		"direct framebuffer destruction also invalidates its image");
	std::puts("Shared SVG cache: context alternation, teardown and address reuse passed");
}
