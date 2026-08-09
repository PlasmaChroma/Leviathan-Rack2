#include "PuffyBodyImageCache.hpp"

#include "PuffyEngine.hpp"
#include "PuffyVisualPalette.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <vector>

namespace puffy_body_cache {
namespace {

enum class ResourceState {
	Empty,
	Building,
	Ready
};

struct CachedImage {
	ResourceState state = ResourceState::Empty;
	int handle = -1;
	int width = 0;
	int height = 0;
};

struct SharedCache {
	std::mutex mutex;
	NVGcontext* activeVg = nullptr;
	std::vector<std::uint8_t> sourcePixels;
	int sourceWidth = 0;
	int sourceHeight = 0;
	bool sourceLoaded = false;
	std::vector<std::uint8_t> pointerSourcePixels;
	int pointerSourceWidth = 0;
	int pointerSourceHeight = 0;
	bool pointerSourceLoaded = false;
	CachedImage transitionAtlas;
	std::array<CachedImage, puffy::kCharacterCount * puffy::kCharacterCount>
		finalBodies {};
	std::array<CachedImage, puffy::kCharacterCount * puffy::kCharacterCount>
		finalPointers {};
};

SharedCache& cache() {
	static SharedCache instance;
	return instance;
}

bool ensureSourcePixels(SharedCache& shared) {
	if (shared.sourceLoaded) {
		return true;
	}
	shared.sourceLoaded = visual_assets::decodeRasterRgba8(
		asset::plugin(pluginInstance, "res/icon/Puffy_Body_NS.png"),
		&shared.sourcePixels,
		&shared.sourceWidth,
		&shared.sourceHeight);
	return shared.sourceLoaded;
}

bool ensurePointerSourcePixels(SharedCache& shared) {
	if (shared.pointerSourceLoaded) {
		return true;
	}
	shared.pointerSourceLoaded = visual_assets::decodeRasterRgba8(
		asset::plugin(pluginInstance, "res/icon/arrow-33p-crop-32c.png"),
		&shared.pointerSourcePixels,
		&shared.pointerSourceWidth,
		&shared.pointerSourceHeight);
	return shared.pointerSourceLoaded;
}

bool selectContext(SharedCache& shared, NVGcontext* vg) {
	if (shared.activeVg == vg) {
		return false;
	}
	const bool reset = shared.activeVg != nullptr;
	shared.activeVg = vg;
	shared.transitionAtlas = {};
	for (CachedImage& image : shared.finalBodies) {
		image = {};
	}
	for (CachedImage& image : shared.finalPointers) {
		image = {};
	}
	return reset;
}

bool imageIsValid(NVGcontext* vg, const CachedImage& image) {
	return image.state == ResourceState::Ready
		&& nvg_gfx_lifecycle::ownedNvgImageSizeMatches(
			vg, image.handle, image.width, image.height);
}

void invalidateCurrentImage(NVGcontext* vg, CachedImage& image) {
	if (vg && image.handle >= 0) {
		nvgDeleteImage(vg, image.handle);
	}
	image = {};
}

std::uint8_t colorByte(float value) {
	return std::uint8_t(clamp(int(value * 255.f + 0.5f), 0, 255));
}

} // namespace

ImageAccess ensureFinalBody(
	NVGcontext* vg,
	int negativeCharacter,
	int positiveCharacter) {
	ImageAccess result;
	if (!vg) {
		return result;
	}
	SharedCache& shared = cache();
	std::lock_guard<std::mutex> lock(shared.mutex);
	result.contextReset = selectContext(shared, vg);
	if (!ensureSourcePixels(shared)) {
		return result;
	}
	negativeCharacter = clamp(
		negativeCharacter, 0, puffy::kCharacterCount - 1);
	positiveCharacter = clamp(
		positiveCharacter, 0, puffy::kCharacterCount - 1);
	CachedImage& cached = shared.finalBodies[size_t(
		negativeCharacter * puffy::kCharacterCount + positiveCharacter)];
	if (cached.state == ResourceState::Ready && !imageIsValid(vg, cached)) {
		invalidateCurrentImage(vg, cached);
	}
	if (imageIsValid(vg, cached)) {
		result.handle = cached.handle;
		result.width = cached.width;
		result.height = cached.height;
		result.cacheHit = true;
		return result;
	}
	if (cached.state == ResourceState::Building) {
		return result;
	}
	cached.state = ResourceState::Building;
	const auto recolorStart = std::chrono::steady_clock::now();
	const NVGcolor negativeTint =
		puffy_visual::characterTint(negativeCharacter);
	const NVGcolor positiveTint =
		puffy_visual::characterTint(positiveCharacter);
	const std::array<std::uint8_t, 6> tint {{
		colorByte(negativeTint.r), colorByte(negativeTint.g),
		colorByte(negativeTint.b), colorByte(positiveTint.r),
		colorByte(positiveTint.g), colorByte(positiveTint.b)
	}};
	std::vector<std::uint8_t> pixels(shared.sourcePixels.size());
	const int denominator = std::max(shared.sourceWidth - 1, 1);
	std::vector<std::array<std::uint8_t, 3>> columnTints(
		size_t(shared.sourceWidth));
	for (int x = 0; x < shared.sourceWidth; ++x) {
		for (int channel = 0; channel < 3; ++channel) {
			columnTints[size_t(x)][size_t(channel)] = std::uint8_t(
				(int(tint[size_t(channel)]) * (denominator - x)
					+ int(tint[size_t(channel + 3)]) * x
					+ denominator / 2) / denominator);
		}
	}
	for (int y = 0; y < shared.sourceHeight; ++y) {
		for (int x = 0; x < shared.sourceWidth; ++x) {
			const size_t pixel =
				(size_t(y) * size_t(shared.sourceWidth) + size_t(x)) * 4u;
			for (int channel = 0; channel < 3; ++channel) {
				pixels[pixel + size_t(channel)] = std::uint8_t(
					(int(shared.sourcePixels[pixel + size_t(channel)])
						* int(columnTints[size_t(x)][size_t(channel)]) + 127) / 255);
			}
			pixels[pixel + 3u] = shared.sourcePixels[pixel + 3u];
		}
	}
	result.recolorNs = std::uint64_t(
		std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - recolorStart).count());
	const auto uploadStart = std::chrono::steady_clock::now();
	const int handle = nvgCreateImageRGBA(
		vg,
		shared.sourceWidth,
		shared.sourceHeight,
		NVG_IMAGE_GENERATE_MIPMAPS,
		pixels.data());
	result.uploadNs = std::uint64_t(
		std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - uploadStart).count());
	if (handle < 0) {
		cached = {};
		return result;
	}
	cached.state = ResourceState::Ready;
	cached.handle = handle;
	cached.width = shared.sourceWidth;
	cached.height = shared.sourceHeight;
	result.handle = handle;
	result.width = cached.width;
	result.height = cached.height;
	result.created = true;
	result.recolored = true;
	return result;
}

ImageAccess ensureFinalPointer(
	NVGcontext* vg,
	int negativeCharacter,
	int positiveCharacter) {
	ImageAccess result;
	if (!vg) {
		return result;
	}
	SharedCache& shared = cache();
	std::lock_guard<std::mutex> lock(shared.mutex);
	result.contextReset = selectContext(shared, vg);
	if (!ensurePointerSourcePixels(shared)) {
		return result;
	}
	negativeCharacter = clamp(
		negativeCharacter, 0, puffy::kCharacterCount - 1);
	positiveCharacter = clamp(
		positiveCharacter, 0, puffy::kCharacterCount - 1);
	CachedImage& cached = shared.finalPointers[size_t(
		negativeCharacter * puffy::kCharacterCount + positiveCharacter)];
	if (cached.state == ResourceState::Ready && !imageIsValid(vg, cached)) {
		invalidateCurrentImage(vg, cached);
	}
	if (imageIsValid(vg, cached)) {
		result.handle = cached.handle;
		result.width = cached.width;
		result.height = cached.height;
		result.cacheHit = true;
		return result;
	}
	if (cached.state == ResourceState::Building) {
		return result;
	}
	cached.state = ResourceState::Building;

	const auto recolorStart = std::chrono::steady_clock::now();
	const NVGcolor negativeTint =
		puffy_visual::characterTint(negativeCharacter);
	const NVGcolor positiveTint =
		puffy_visual::characterTint(positiveCharacter);
	const std::array<std::uint8_t, 6> tint {{
		colorByte(negativeTint.r), colorByte(negativeTint.g),
		colorByte(negativeTint.b), colorByte(positiveTint.r),
		colorByte(positiveTint.g), colorByte(positiveTint.b)
	}};
	std::vector<std::uint8_t> pixels(shared.pointerSourcePixels.size());
	const int denominator = std::max(shared.pointerSourceWidth - 1, 1);
	std::vector<std::array<std::uint8_t, 3>> columnTints(
		size_t(shared.pointerSourceWidth));
	for (int x = 0; x < shared.pointerSourceWidth; ++x) {
		for (int channel = 0; channel < 3; ++channel) {
			columnTints[size_t(x)][size_t(channel)] = std::uint8_t(
				(int(tint[size_t(channel)]) * (denominator - x)
					+ int(tint[size_t(channel + 3)]) * x
					+ denominator / 2) / denominator);
		}
	}
	for (int y = 0; y < shared.pointerSourceHeight; ++y) {
		for (int x = 0; x < shared.pointerSourceWidth; ++x) {
			const size_t pixel =
				(size_t(y) * size_t(shared.pointerSourceWidth) + size_t(x)) * 4u;
			for (int channel = 0; channel < 3; ++channel) {
				pixels[pixel + size_t(channel)] = std::uint8_t(
					(int(shared.pointerSourcePixels[pixel + size_t(channel)])
						* int(columnTints[size_t(x)][size_t(channel)]) + 127) / 255);
			}
			pixels[pixel + 3u] = shared.pointerSourcePixels[pixel + 3u];
		}
	}
	result.recolorNs = std::uint64_t(
		std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - recolorStart).count());
	const auto uploadStart = std::chrono::steady_clock::now();
	const int handle = nvgCreateImageRGBA(
		vg,
		shared.pointerSourceWidth,
		shared.pointerSourceHeight,
		NVG_IMAGE_GENERATE_MIPMAPS,
		pixels.data());
	result.uploadNs = std::uint64_t(
		std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - uploadStart).count());
	if (handle < 0) {
		cached = {};
		return result;
	}
	cached.state = ResourceState::Ready;
	cached.handle = handle;
	cached.width = shared.pointerSourceWidth;
	cached.height = shared.pointerSourceHeight;
	result.handle = handle;
	result.width = cached.width;
	result.height = cached.height;
	result.created = true;
	result.recolored = true;
	return result;
}

ImageAccess ensureTransitionAtlas(NVGcontext* vg) {
	ImageAccess result;
	if (!vg) {
		return result;
	}
	SharedCache& shared = cache();
	std::lock_guard<std::mutex> lock(shared.mutex);
	result.contextReset = selectContext(shared, vg);
	if (!ensureSourcePixels(shared)) {
		return result;
	}
	CachedImage& cached = shared.transitionAtlas;
	if (cached.state == ResourceState::Ready && !imageIsValid(vg, cached)) {
		invalidateCurrentImage(vg, cached);
	}
	if (imageIsValid(vg, cached)) {
		result.handle = cached.handle;
		result.width = cached.width;
		result.height = cached.height;
		result.cacheHit = true;
		return result;
	}
	if (cached.state == ResourceState::Building) {
		return result;
	}
	cached.state = ResourceState::Building;
	const int atlasWidth = shared.sourceWidth * 3;
	const int atlasHeight = shared.sourceHeight;
	std::vector<std::uint8_t> pixels(
		size_t(atlasWidth) * size_t(atlasHeight) * 4u, 0u);
	const int denominator = std::max(shared.sourceWidth - 1, 1);
	for (int y = 0; y < shared.sourceHeight; ++y) {
		for (int x = 0; x < shared.sourceWidth; ++x) {
			const size_t source =
				(size_t(y) * size_t(shared.sourceWidth) + size_t(x)) * 4u;
			const int alpha = int(shared.sourcePixels[source + 3u]);
			const int weights[2] = {denominator - x, x};
			const size_t knockout =
				(size_t(y) * size_t(atlasWidth) + size_t(x)) * 4u;
			pixels[knockout + 3u] = std::uint8_t(alpha);
			for (int side = 0; side < 2; ++side) {
				const size_t destination =
					(size_t(y) * size_t(atlasWidth)
						+ size_t((side + 1) * shared.sourceWidth + x)) * 4u;
				const int weightedAlpha =
					(alpha * weights[side] + denominator / 2) / denominator;
				for (int channel = 0; channel < 3; ++channel) {
					pixels[destination + size_t(channel)] = std::uint8_t(
						(int(shared.sourcePixels[source + size_t(channel)])
							* weightedAlpha + 127) / 255);
				}
				pixels[destination + 3u] = std::uint8_t(weightedAlpha);
			}
		}
	}
	const int handle = nvgCreateImageRGBA(
		vg,
		atlasWidth,
		atlasHeight,
		NVG_IMAGE_PREMULTIPLIED | NVG_IMAGE_GENERATE_MIPMAPS,
		pixels.data());
	if (handle < 0) {
		cached = {};
		return result;
	}
	cached.state = ResourceState::Ready;
	cached.handle = handle;
	cached.width = atlasWidth;
	cached.height = atlasHeight;
	result.handle = handle;
	result.width = atlasWidth;
	result.height = atlasHeight;
	result.created = true;
	return result;
}

} // namespace puffy_body_cache
