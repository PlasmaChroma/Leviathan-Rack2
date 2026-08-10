#include "VisualAssets.hpp"
#include "../NvgGraphicsLifecycle.hpp"
#include "../PanelSvgUtils.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <stb_image.h>

namespace visual_assets {
namespace {

uint32_t readPngUint32(const unsigned char* p) {
	return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
}

unsigned char pngPaethPredictor(unsigned char left, unsigned char up, unsigned char upperLeft) {
	int p = int(left) + int(up) - int(upperLeft);
	int pa = std::abs(p - int(left));
	int pb = std::abs(p - int(up));
	int pc = std::abs(p - int(upperLeft));
	return pa <= pb && pa <= pc ? left : (pb <= pc ? up : upperLeft);
}

// Rack's bundled stb_image predates its fix for filtered 1/2/4-bit PNGs.
// Expand that narrowly identified format ourselves before creating the
// NanoVG texture, while leaving all other formats on NanoVG's normal path.
int createLowBitIndexedPngMipmapImage(NVGcontext* vg, const std::string& path) {
	std::ifstream in(path.c_str(), std::ios::in | std::ios::binary);
	if (!in) {
		return -1;
	}
	std::array<unsigned char, 29> header {};
	in.read(reinterpret_cast<char*>(header.data()), header.size());
	static const unsigned char kPngSignature[8] = {0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a};
	if (in.gcount() != std::streamsize(header.size()) ||
		!std::equal(kPngSignature, kPngSignature + 8, header.begin()) ||
		readPngUint32(header.data() + 8) != 13 ||
		std::memcmp(header.data() + 12, "IHDR", 4) != 0) {
		return -1;
	}

	uint32_t width = readPngUint32(header.data() + 16);
	uint32_t height = readPngUint32(header.data() + 20);
	unsigned int bitDepth = header[24];
	unsigned int colorType = header[25];
	unsigned int compression = header[26];
	unsigned int filterMethod = header[27];
	unsigned int interlace = header[28];
	constexpr uint64_t kMaxPixels = 64ull * 1024ull * 1024ull;
	if (colorType != 3 || (bitDepth != 1 && bitDepth != 2 && bitDepth != 4) ||
		compression != 0 || filterMethod != 0 || interlace != 0 ||
		width == 0 || height == 0 || uint64_t(width) * uint64_t(height) > kMaxPixels) {
		return -1;
	}

	in.seekg(0, std::ios::end);
	std::streamoff fileSize = in.tellg();
	constexpr std::streamoff kMaxPngBytes = 256 * 1024 * 1024;
	if (fileSize < 29 || fileSize > kMaxPngBytes) {
		return -1;
	}
	in.seekg(0, std::ios::beg);
	std::vector<unsigned char> encoded(static_cast<size_t>(fileSize));
	in.read(reinterpret_cast<char*>(encoded.data()), fileSize);
	if (!in) {
		return -1;
	}

	std::vector<unsigned char> palette;
	std::vector<unsigned char> alpha;
	std::vector<unsigned char> compressed;
	size_t offset = 8;
	while (offset + 12 <= encoded.size()) {
		uint32_t chunkSize = readPngUint32(encoded.data() + offset);
		if (uint64_t(offset) + 12ull + uint64_t(chunkSize) > encoded.size()) {
			return -1;
		}
		const unsigned char* chunkType = encoded.data() + offset + 4;
		const unsigned char* chunkData = encoded.data() + offset + 8;
		if (std::memcmp(chunkType, "PLTE", 4) == 0) {
			if (chunkSize == 0 || chunkSize > 256 * 3 || chunkSize % 3 != 0) {
				return -1;
			}
			palette.assign(chunkData, chunkData + chunkSize);
		} else if (std::memcmp(chunkType, "tRNS", 4) == 0) {
			if (chunkSize > 256) {
				return -1;
			}
			alpha.assign(chunkData, chunkData + chunkSize);
		} else if (std::memcmp(chunkType, "IDAT", 4) == 0) {
			if (compressed.size() + uint64_t(chunkSize) > uint64_t(kMaxPngBytes)) {
				return -1;
			}
			compressed.insert(compressed.end(), chunkData, chunkData + chunkSize);
		} else if (std::memcmp(chunkType, "IEND", 4) == 0) {
			break;
		}
		offset += size_t(chunkSize) + 12;
	}
	if (palette.empty() || compressed.empty()) {
		return -1;
	}

	size_t rowBytes = (size_t(width) * bitDepth + 7) / 8;
	size_t filteredSize = (rowBytes + 1) * size_t(height);
	if (filteredSize > size_t(std::numeric_limits<int>::max()) ||
		compressed.size() > size_t(std::numeric_limits<int>::max())) {
		return -1;
	}
	std::vector<unsigned char> filtered(filteredSize);
	int decodedBytes = stbi_zlib_decode_buffer(
		reinterpret_cast<char*>(filtered.data()), int(filtered.size()),
		reinterpret_cast<const char*>(compressed.data()), int(compressed.size()));
	if (decodedBytes != int(filtered.size())) {
		return -1;
	}

	std::vector<unsigned char> unpacked(rowBytes * size_t(height));
	for (uint32_t y = 0; y < height; ++y) {
		const unsigned char* source = filtered.data() + size_t(y) * (rowBytes + 1);
		unsigned int rowFilter = *source++;
		if (rowFilter > 4) {
			return -1;
		}
		unsigned char* row = unpacked.data() + size_t(y) * rowBytes;
		const unsigned char* prior = y > 0 ? row - rowBytes : nullptr;
		for (size_t x = 0; x < rowBytes; ++x) {
			unsigned char left = x > 0 ? row[x - 1] : 0;
			unsigned char up = prior ? prior[x] : 0;
			unsigned char upperLeft = prior && x > 0 ? prior[x - 1] : 0;
			switch (rowFilter) {
			case 0:
				row[x] = source[x];
				break;
			case 1:
				row[x] = source[x] + left;
				break;
			case 2:
				row[x] = source[x] + up;
				break;
			case 3:
				row[x] = source[x] +
					((static_cast<unsigned int>(left) + static_cast<unsigned int>(up)) >> 1);
				break;
			case 4:
				row[x] = source[x] + pngPaethPredictor(left, up, upperLeft);
				break;
			}
		}
	}

	size_t paletteEntries = palette.size() / 3;
	unsigned int indexMask = (1u << bitDepth) - 1u;
	std::vector<unsigned char> rgba(size_t(width) * size_t(height) * 4);
	for (uint32_t y = 0; y < height; ++y) {
		const unsigned char* row = unpacked.data() + size_t(y) * rowBytes;
		for (uint32_t x = 0; x < width; ++x) {
			size_t bitOffset = size_t(x) * bitDepth;
			unsigned int shift = 8u - bitDepth - unsigned(bitOffset & 7u);
			unsigned int index = (row[bitOffset >> 3] >> shift) & indexMask;
			if (index >= paletteEntries) {
				return -1;
			}
			size_t destination = (size_t(y) * width + x) * 4;
			rgba[destination] = palette[index * 3];
			rgba[destination + 1] = palette[index * 3 + 1];
			rgba[destination + 2] = palette[index * 3 + 2];
			rgba[destination + 3] = index < alpha.size() ? alpha[index] : 255;
		}
	}

	return nvgCreateImageRGBA(vg, int(width), int(height), NVG_IMAGE_GENERATE_MIPMAPS, rgba.data());
}

struct AspectFitRasterImageWidget : TransparentWidget {
	std::string path;
	bool flipHorizontal = false;
	float opacity = 1.f;

	AspectFitRasterImageWidget(std::string path, bool flipHorizontal, float opacity)
		: path(std::move(path)), flipHorizontal(flipHorizontal), opacity(opacity) {
	}

	void draw(const DrawArgs& args) override {
		if (path.empty() || box.size.x <= 1.f || box.size.y <= 1.f) {
			return;
		}
		const std::string fullPath = asset::plugin(pluginInstance, path);
		std::shared_ptr<window::Image> image = APP->window->loadImage(fullPath);
		if (!image || image->handle < 0) {
			return;
		}
		int imageHandle = loadRasterMipmapHandle(args.vg, image, fullPath);
		if (imageHandle < 0) {
			imageHandle = image->handle;
		}

		int imageW = 0;
		int imageH = 0;
		nvgImageSize(args.vg, imageHandle, &imageW, &imageH);
		if (imageW <= 0 || imageH <= 0) {
			return;
		}

		const float aspect = float(imageW) / float(imageH);
		float drawW = box.size.x;
		float drawH = drawW / aspect;
		if (drawH > box.size.y) {
			drawH = box.size.y;
			drawW = drawH * aspect;
		}
		const float x = 0.5f * (box.size.x - drawW);
		const float y = 0.5f * (box.size.y - drawH);
		nvgSave(args.vg);
		if (flipHorizontal) {
			nvgTranslate(args.vg, box.size.x, 0.f);
			nvgScale(args.vg, -1.f, 1.f);
		}
		NVGpaint paint = nvgImagePattern(args.vg, x, y, drawW, drawH, 0.f, imageHandle, clamp(opacity, 0.f, 1.f));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, x, y, drawW, drawH);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
};

} // namespace

bool decodeRasterRgba8(
	const std::string& fullPath,
	std::vector<std::uint8_t>* rgba,
	int* width,
	int* height) {
	if (!rgba || !width || !height || fullPath.empty()) {
		return false;
	}
	int decodedWidth = 0;
	int decodedHeight = 0;
	int sourceChannels = 0;
	stbi_uc* decoded = stbi_load(
		fullPath.c_str(), &decodedWidth, &decodedHeight, &sourceChannels, 4);
	if (!decoded || decodedWidth <= 0 || decodedHeight <= 0) {
		if (decoded) {
			stbi_image_free(decoded);
		}
		return false;
	}
	const size_t byteCount =
		size_t(decodedWidth) * size_t(decodedHeight) * size_t(4);
	rgba->assign(decoded, decoded + byteCount);
	stbi_image_free(decoded);
	*width = decodedWidth;
	*height = decodedHeight;
	return true;
}

int loadRasterMipmapHandle(
	NVGcontext* vg,
	std::shared_ptr<window::Image> lifecycleImage,
	const std::string& fullPath
) {
	struct Entry {
		NVGcontext* vg = nullptr;
		int handle = -1;
		int lifecycleHandle = -1;
		std::weak_ptr<window::Image> lifecycleImage;
	};
	struct Cache {
		std::unordered_map<std::string, Entry> entries;
		NVGcontext* activeVg = nullptr;
		unsigned long long useCounter = 0ull;
	};
	static Cache cache;

	if (!vg || fullPath.empty() || !lifecycleImage || lifecycleImage->handle < 0) {
		return -1;
	}
	if (nvg_gfx_lifecycle::clearCacheOnContextSwitch(vg, cache.activeVg, &cache.useCounter)) {
		cache.entries.clear();
	}

	auto it = cache.entries.find(fullPath);
	if (it != cache.entries.end()) {
		std::shared_ptr<window::Image> cachedLifecycleImage = it->second.lifecycleImage.lock();
		if (it->second.vg == vg && it->second.handle >= 0 &&
			it->second.lifecycleHandle == lifecycleImage->handle && cachedLifecycleImage == lifecycleImage) {
			return it->second.handle;
		}
		if (it->second.vg == vg && it->second.handle >= 0 && cachedLifecycleImage) {
			nvgDeleteImage(vg, it->second.handle);
		}
		cache.entries.erase(it);
	}

	int handle = createLowBitIndexedPngMipmapImage(vg, fullPath);
	if (handle < 0) {
		handle = nvgCreateImage(vg, fullPath.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
	}
	if (handle < 0) {
		return -1;
	}

	Entry entry;
	entry.vg = vg;
	entry.handle = handle;
	entry.lifecycleHandle = lifecycleImage->handle;
	entry.lifecycleImage = lifecycleImage;
	cache.entries[fullPath] = entry;
	return handle;
}

Widget* createAspectFitRasterImageWidget(
	const char* imageAssetPath,
	math::Rect rectMm,
	bool flipHorizontal,
	float opacity) {
	AspectFitRasterImageWidget* image = new AspectFitRasterImageWidget(
		imageAssetPath ? imageAssetPath : "", flipHorizontal, opacity);
	image->box.pos = mm2px(rectMm.pos);
	image->box.size = mm2px(rectMm.size);
	return image;
}

int addMirroredPanelRasterImages(
	Widget* parent,
	const std::string& panelPath,
	const char* imageAssetPath,
	const char* leftAnchorId,
	const char* rightAnchorId,
	float opacity) {
	if (!parent || panelPath.empty() || !imageAssetPath || imageAssetPath[0] == '\0') {
		return 0;
	}
	const float clampedOpacity = clamp(opacity, 0.f, 1.f);
	int added = 0;
	auto addAtAnchor = [&](const char* anchorId, bool flipHorizontal) {
		if (!anchorId || anchorId[0] == '\0') {
			return;
		}
		math::Rect rectMm;
		if (!panel_svg::loadRectFromSvgMm(panelPath, anchorId, &rectMm) ||
			rectMm.size.x <= 0.f || rectMm.size.y <= 0.f) {
			return;
		}
		parent->addChild(createAspectFitRasterImageWidget(
			imageAssetPath, rectMm, flipHorizontal, clampedOpacity));
		++added;
	};
	addAtAnchor(leftAnchorId, false);
	if (!leftAnchorId || !rightAnchorId ||
		std::string(leftAnchorId) != std::string(rightAnchorId)) {
		addAtAnchor(rightAnchorId, true);
	}
	return added;
}

int addPerfectWavePanelBranding(
	Widget* parent,
	const std::string& panelPath,
	float opacity) {
	return addMirroredPanelRasterImages(
		parent,
		panelPath,
		"res/icon/PerfectWave_Tiny.png",
		"BRANDING_WAVE_LEFT_RASTER",
		"BRANDING_WAVE_RIGHT_RASTER",
		opacity);
}

int addPerfectWaveSoloPanelBranding(
	Widget* parent,
	const std::string& panelPath,
	float opacity) {
	return addMirroredPanelRasterImages(
		parent,
		panelPath,
		"res/icon/PerfectWave_Tiny.png",
		"BRANDING_WAVE_SOLO_RASTER",
		nullptr,
		opacity);
}

int addCompactLeviathanLogoBranding(
	Widget* parent,
	const std::string& panelPath,
	float opacity) {
	if (!parent) {
		return 0;
	}
	// Proc, Undertow, and TD.Scope share the same 8 HP panel dimensions.
	math::Rect logoRectMm(
		Vec(3.960335f, 118.43102f),
		Vec(32.71933f, 12.24054f));
	panel_svg::loadRectFromSvgMm(
		panelPath, "BRANDING_LEVIATHAN_LOGO_RASTER", &logoRectMm);
	parent->addChild(createAspectFitRasterImageWidget(
		"res/icon/Leviathan_Logo_S2.png",
		logoRectMm,
		false,
		clamp(opacity, 0.f, 1.f)));
	return 1;
}

} // namespace visual_assets
