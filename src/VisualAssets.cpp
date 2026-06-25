#include "VisualAssets.hpp"
#include "MathHelpers.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "PanelSvgUtils.hpp"

#include <chrono>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>

namespace visual_assets {

namespace {

thread_local uint64_t gEclipseShadowDrawNs = 0u;
thread_local uint64_t gEclipseShadowDrawCount = 0u;

} // namespace

std::shared_ptr<window::Svg> loadPluginSvgCached(const char* path) {
	static std::map<std::string, std::shared_ptr<window::Svg>> cache;
	const std::string key = path ? path : "";
	auto it = cache.find(key);
	if (it != cache.end()) {
		return it->second;
	}
	std::shared_ptr<window::Svg> svg = Svg::load(asset::plugin(pluginInstance, key));
	cache[key] = svg;
	return svg;
}

namespace {

struct SvgRect3DEffectWidget : TransparentWidget {
	float edgeMarginPx = 0.f;
	NVGcolor baseColor = nvgRGB(87, 64, 191);
	NVGcolor shadowBaseColor = nvgRGB(87, 64, 191);

	static NVGcolor mixColor(NVGcolor a, NVGcolor b, float t, float alphaScale = 1.f) {
		t = clamp(t, 0.f, 1.f);
		NVGcolor out;
		out.r = a.r + (b.r - a.r) * t;
		out.g = a.g + (b.g - a.g) * t;
		out.b = a.b + (b.b - a.b) * t;
		out.a = clamp((a.a + (b.a - a.a) * t) * alphaScale, 0.f, 1.f);
		return out;
	}

	void draw(const DrawArgs& args) override {
		const float x = edgeMarginPx;
		const float y = edgeMarginPx;
		const float w = box.size.x - 2.f * edgeMarginPx;
		const float h = box.size.y - 2.f * edgeMarginPx;
		if (w <= 1.f || h <= 1.f) {
			return;
		}

		const float radius = clamp(std::min(w, h) * 0.055f, 2.f, 8.f);
		const float bevel = clamp(std::min(w, h) * 0.010f, 0.38f, 1.35f);
		const NVGcolor lightColor = mixColor(baseColor, nvgRGB(255, 255, 255), 0.18f, 0.46f);
		const NVGcolor shadowColor = mixColor(shadowBaseColor, nvgRGB(0, 0, 0), 0.56f, 0.80f);
		const NVGcolor innerShadowColor = mixColor(shadowBaseColor, nvgRGB(0, 0, 0), 0.40f, 0.44f);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x, y, w, h, radius);
		nvgStrokeWidth(args.vg, bevel * 0.82f);
		NVGpaint outerPaint = nvgLinearGradient(
			args.vg,
			x,
			y,
			x,
			y + h,
			lightColor,
			shadowColor);
		nvgStrokePaint(args.vg, outerPaint);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + bevel * 0.42f, y + bevel * 0.42f, std::max(0.f, w - bevel * 0.84f), std::max(0.f, h - bevel * 0.84f), std::max(0.f, radius - bevel * 0.42f));
		nvgStrokeWidth(args.vg, std::max(0.20f, bevel * 0.18f));
		nvgStrokeColor(args.vg, innerShadowColor);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x + radius, y + h);
		nvgLineTo(args.vg, x + w - radius, y + h);
		nvgStrokeColor(args.vg, shadowColor);
		nvgStrokeWidth(args.vg, std::max(0.22f, bevel * 0.22f));
		nvgStroke(args.vg);

		nvgSave(args.vg);
		nvgIntersectScissor(args.vg, x - edgeMarginPx, y - edgeMarginPx, w + 2.f * edgeMarginPx, h + 2.f * edgeMarginPx);
		nvgBeginPath(args.vg);
		const float sheenInset = std::min(w * 0.18f, radius + bevel * 2.2f);
		nvgRect(args.vg, x + sheenInset, y - bevel * 0.45f, std::max(1.f, w - 2.f * sheenInset), bevel * 0.90f);
		NVGpaint sheenPaint = nvgLinearGradient(
			args.vg,
			x,
			y - bevel,
			x,
			y + bevel,
			mixColor(baseColor, nvgRGB(255, 255, 255), 0.24f, 0.28f),
			nvgRGBA(255, 255, 255, 0));
		nvgFillPaint(args.vg, sheenPaint);
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
};

int loadRasterMipmapHandle(NVGcontext* vg, std::shared_ptr<window::Image> lifecycleImage, const std::string& fullPath) {
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

	int handle = nvgCreateImage(vg, fullPath.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
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

struct AspectFitRasterImageWidget : TransparentWidget {
	std::string path;
	float opacity = 1.f;

	AspectFitRasterImageWidget(std::string path, float opacity)
		: path(std::move(path)), opacity(opacity) {
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
		NVGpaint paint = nvgImagePattern(args.vg, x, y, drawW, drawH, 0.f, imageHandle, clamp(opacity, 0.f, 1.f));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, x, y, drawW, drawH);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}
};

struct PreviewFrameEnhancementWidget : TransparentWidget {
	float outsideMarginPx = 0.f;

	void draw(const DrawArgs& args) override {
		const float x = outsideMarginPx;
		const float y = outsideMarginPx;
		const float w = box.size.x - 2.f * outsideMarginPx;
		const float h = box.size.y - 2.f * outsideMarginPx;
		if (w <= 2.f || h <= 2.f) {
			return;
		}

		nvgSave(args.vg);

		nvgBeginPath(args.vg);
		nvgRect(args.vg, x - 0.55f, y - 0.55f, w + 1.1f, h + 1.1f);
		nvgStrokeWidth(args.vg, 1.0f);
		nvgStrokeColor(args.vg, nvgRGBA(28, 202, 216, 115));
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x - 0.15f, y + h + 0.45f);
		nvgLineTo(args.vg, x + w + 0.45f, y + h + 0.45f);
		nvgLineTo(args.vg, x + w + 0.45f, y - 0.15f);
		nvgStrokeWidth(args.vg, 1.7f);
		nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 96));
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x - 0.6f, y + h + 0.25f);
		nvgLineTo(args.vg, x - 0.6f, y - 0.6f);
		nvgLineTo(args.vg, x + w + 0.25f, y - 0.6f);
		nvgStrokeWidth(args.vg, 0.85f);
		nvgStrokeColor(args.vg, nvgRGBA(92, 245, 255, 42));
		nvgStroke(args.vg);

		nvgRestore(args.vg);
	}
};

struct PanelSurfaceEffectWidget : TransparentWidget {
	struct GlassRectArt {
		math::Rect rectPx;
		float radiusPx = 0.f;
		NVGcolor baseColor = nvgRGB(87, 64, 191);
	};

	std::vector<GlassRectArt> glassRects;
	std::vector<math::Rect> screenRectsPx;

	static bool rectsIntersect(const math::Rect& a, const math::Rect& b) {
		return a.pos.x < b.pos.x + b.size.x
			&& a.pos.x + a.size.x > b.pos.x
			&& a.pos.y < b.pos.y + b.size.y
			&& a.pos.y + a.size.y > b.pos.y;
	}

	static math::Rect rectIntersection(const math::Rect& a, const math::Rect& b) {
		const float x0 = std::max(a.pos.x, b.pos.x);
		const float y0 = std::max(a.pos.y, b.pos.y);
		const float x1 = std::min(a.pos.x + a.size.x, b.pos.x + b.size.x);
		const float y1 = std::min(a.pos.y + a.size.y, b.pos.y + b.size.y);
		return math::Rect(Vec(x0, y0), Vec(std::max(0.f, x1 - x0), std::max(0.f, y1 - y0)));
	}

	static void subtractRect(std::vector<math::Rect>& pieces, const math::Rect& cut) {
		std::vector<math::Rect> next;
		next.reserve(pieces.size() + 3u);
		for (const math::Rect& piece : pieces) {
			if (!rectsIntersect(piece, cut)) {
				next.push_back(piece);
				continue;
			}
			const math::Rect inter = rectIntersection(piece, cut);
			const float px0 = piece.pos.x;
			const float py0 = piece.pos.y;
			const float px1 = piece.pos.x + piece.size.x;
			const float py1 = piece.pos.y + piece.size.y;
			const float ix0 = inter.pos.x;
			const float iy0 = inter.pos.y;
			const float ix1 = inter.pos.x + inter.size.x;
			const float iy1 = inter.pos.y + inter.size.y;
			auto addPiece = [&next](float x0, float y0, float x1, float y1) {
				if (x1 - x0 > 0.5f && y1 - y0 > 0.5f) {
					next.push_back(math::Rect(Vec(x0, y0), Vec(x1 - x0, y1 - y0)));
				}
			};
			addPiece(px0, py0, px1, iy0);
			addPiece(px0, iy1, px1, py1);
			addPiece(px0, iy0, ix0, iy1);
			addPiece(ix1, iy0, px1, iy1);
		}
		pieces = std::move(next);
	}

	void drawGlassRectPiece(const DrawArgs& args, const GlassRectArt& glass, const math::Rect& piece) {
		const float x = glass.rectPx.pos.x;
		const float y = glass.rectPx.pos.y;
		const float w = glass.rectPx.size.x;
		const float h = glass.rectPx.size.y;
		if (!(w > 2.f && h > 2.f && piece.size.x > 0.5f && piece.size.y > 0.5f)) {
			return;
		}

		const float sourceRadius = glass.radiusPx > 0.f ? glass.radiusPx : std::min(std::min(w, h) * 0.085f, 8.0f);
		const float r = clamp(sourceRadius, 0.f, std::min(w, h) * 0.5f);
		const NVGcolor base = glass.baseColor;
		const NVGcolor cyan = nvgRGB(0x1c, 0xcc, 0xd9);
		const NVGcolor violet = nvgRGB(0x7a, 0x5c, 0xff);

		nvgSave(args.vg);
		nvgScissor(args.vg, piece.pos.x, piece.pos.y, piece.size.x, piece.size.y);

		NVGpaint outerGlow = nvgBoxGradient(args.vg, x - 1.5f, y - 1.5f, w + 3.0f, h + 3.0f, r + 2.0f, 7.0f,
			nvgRGBAf(base.r, base.g, base.b, 0.075f), nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x - 1.5f, y - 1.5f, w + 3.0f, h + 3.0f, r + 2.0f);
		nvgFillPaint(args.vg, outerGlow);
		nvgFill(args.vg);

		NVGpaint glassFill = nvgLinearGradient(args.vg, x, y, x, y + h,
			nvgRGBA(255, 255, 255, 14), nvgRGBAf(base.r, base.g, base.b, 0.04f));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 0.6f, y + 0.6f, w - 1.2f, h - 1.2f, r);
		nvgFillPaint(args.vg, glassFill);
		nvgFill(args.vg);

		nvgSave(args.vg);
		nvgScissor(args.vg, x + 1.f, y + 1.f, w - 2.f, h - 2.f);
		NVGpaint sheen = nvgLinearGradient(args.vg, x + w * 0.12f, y + h * 0.05f, x + w * 0.55f, y + h * 0.62f,
			nvgRGBA(255, 255, 255, 10), nvgRGBA(255, 255, 255, 0));
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x + w * 0.06f, y);
		nvgLineTo(args.vg, x + w * 0.23f, y);
		nvgLineTo(args.vg, x + w * 0.64f, y + h);
		nvgLineTo(args.vg, x + w * 0.46f, y + h);
		nvgClosePath(args.vg);
		nvgFillPaint(args.vg, sheen);
		nvgFill(args.vg);
		nvgRestore(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 0.75f, y + 0.75f, w - 1.5f, h - 1.5f, r);
		nvgStrokeWidth(args.vg, 0.85f);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 17));
		nvgStroke(args.vg);

		NVGpaint edge = nvgLinearGradient(args.vg, x, y, x + w, y + h,
			nvgRGBAf(violet.r, violet.g, violet.b, 0.16f), nvgRGBAf(cyan.r, cyan.g, cyan.b, 0.12f));
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 1.35f, y + 1.35f, w - 2.7f, h - 2.7f, std::max(1.f, r - 1.f));
		nvgStrokeWidth(args.vg, 0.55f);
		nvgStrokePaint(args.vg, edge);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x + r + 2.f, y + 2.2f);
		nvgLineTo(args.vg, x + w - r - 2.f, y + 2.2f);
		nvgStrokeWidth(args.vg, 0.8f);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 24));
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}

	void drawGlassRect(const DrawArgs& args, const GlassRectArt& glass) {
		std::vector<math::Rect> pieces;
		pieces.push_back(glass.rectPx);
		for (const math::Rect& screen : screenRectsPx) {
			subtractRect(pieces, screen);
			if (pieces.empty()) {
				break;
			}
		}
		for (const math::Rect& piece : pieces) {
			drawGlassRectPiece(args, glass, piece);
		}
	}

	void drawScreenGrid(const DrawArgs& args, const math::Rect& screen) {
		const float x = screen.pos.x;
		const float y = screen.pos.y;
		const float w = screen.size.x;
		const float h = screen.size.y;
		if (!(w > 4.f && h > 4.f)) {
			return;
		}

		nvgSave(args.vg);
		nvgScissor(args.vg, x, y, w, h);
		const int majorCols = std::max(3, int(std::round(w / 16.0f)));
		const int majorRows = std::max(3, int(std::round(h / 16.0f)));
		const int minorSubdivisions = 4;
		const float majorX = w / float(majorCols);
		const float majorY = h / float(majorRows);

		nvgBeginPath(args.vg);
		for (int col = 0; col < majorCols; ++col) {
			const float cellX = x + float(col) * majorX;
			for (int sub = 1; sub < minorSubdivisions; ++sub) {
				const float gx = cellX + majorX * (float(sub) / float(minorSubdivisions));
				nvgMoveTo(args.vg, gx, y);
				nvgLineTo(args.vg, gx, y + h);
			}
		}
		for (int row = 0; row < majorRows; ++row) {
			const float cellY = y + float(row) * majorY;
			for (int sub = 1; sub < minorSubdivisions; ++sub) {
				const float gy = cellY + majorY * (float(sub) / float(minorSubdivisions));
				nvgMoveTo(args.vg, x, gy);
				nvgLineTo(args.vg, x + w, gy);
			}
		}
		nvgStrokeWidth(args.vg, 0.38f);
		nvgStrokeColor(args.vg, nvgRGBA(0x1c, 0xcc, 0xd9, 30));
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		for (int col = 1; col < majorCols; ++col) {
			const float gx = x + float(col) * majorX;
			nvgMoveTo(args.vg, gx, y);
			nvgLineTo(args.vg, gx, y + h);
		}
		for (int row = 1; row < majorRows; ++row) {
			const float gy = y + float(row) * majorY;
			nvgMoveTo(args.vg, x, gy);
			nvgLineTo(args.vg, x + w, gy);
		}
		nvgStrokeWidth(args.vg, 0.55f);
		nvgStrokeColor(args.vg, nvgRGBA(0x72, 0x8d, 0xff, 46));
		nvgStroke(args.vg);

		NVGpaint vignette = nvgBoxGradient(args.vg, x + 1.f, y + 1.f, w - 2.f, h - 2.f, 1.5f, 9.0f,
			nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 132));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, x, y, w, h);
		nvgFillPaint(args.vg, vignette);
		nvgFill(args.vg);

		NVGpaint edgeGlow = nvgBoxGradient(args.vg, x + 0.5f, y + 0.5f, w - 1.f, h - 1.f, 1.5f, 4.0f,
			nvgRGBA(0x1c, 0xcc, 0xd9, 78), nvgRGBA(0x1c, 0xcc, 0xd9, 0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, x, y, w, h);
		nvgStrokeWidth(args.vg, 1.1f);
		nvgStrokePaint(args.vg, edgeGlow);
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}

	void draw(const DrawArgs& args) override {
		for (const GlassRectArt& glass : glassRects) {
			drawGlassRect(args, glass);
		}
		for (const math::Rect& screen : screenRectsPx) {
			drawScreenGrid(args, screen);
		}
	}
};

struct PanelSurfaceEffectDefinition {
	std::vector<PanelSurfaceEffectWidget::GlassRectArt> glassRects;
	std::vector<math::Rect> screenRectsPx;
};

math::Rect insetRectMm(math::Rect rect, float insetMm) {
	rect.pos.x += insetMm;
	rect.pos.y += insetMm;
	rect.size.x = std::max(0.f, rect.size.x - 2.f * insetMm);
	rect.size.y = std::max(0.f, rect.size.y - 2.f * insetMm);
	return rect;
}

PanelSurfaceEffectDefinition loadPanelSurfaceEffectDefinition(const std::string& svgPath) {
	PanelSurfaceEffectDefinition def;
	std::vector<panel_svg::SvgRectMatch> glassMatches;
	if (panel_svg::findRectsInGroupsWithIdSubstringMm(svgPath, "glass", &glassMatches)) {
		def.glassRects.reserve(glassMatches.size());
		for (const panel_svg::SvgRectMatch& match : glassMatches) {
			PanelSurfaceEffectWidget::GlassRectArt art;
			art.rectPx = math::Rect(mm2px(match.rect.pos), mm2px(match.rect.size));
			if (match.hasCornerRadius) {
				const Vec radiusPx = mm2px(match.cornerRadius);
				art.radiusPx = std::min(radiusPx.x, radiusPx.y);
			}
			if (match.hasFillColor) {
				art.baseColor = match.fillColor;
			}
			def.glassRects.push_back(art);
		}
	}

	std::vector<panel_svg::SvgRectMatch> screenMatches;
	if (panel_svg::findRectsInGroupsWithIdSubstringMm(svgPath, "screen", &screenMatches)) {
		def.screenRectsPx.reserve(screenMatches.size());
		for (const panel_svg::SvgRectMatch& match : screenMatches) {
			math::Rect screenRectMm = insetRectMm(match.rect, 0.2f);
			def.screenRectsPx.push_back(math::Rect(mm2px(screenRectMm.pos), mm2px(screenRectMm.size)));
		}
	}
	return def;
}

} // namespace

int loadPluginRasterMipmapHandle(
	NVGcontext* vg,
	std::shared_ptr<window::Image> lifecycleImage,
	const std::string& fullPath
) {
	return loadRasterMipmapHandle(vg, std::move(lifecycleImage), fullPath);
}

Widget* createSvgRect3DEffectWidget(math::Rect rectMm) {
	return createSvgRect3DEffectWidget(rectMm, nvgRGB(87, 64, 191));
}

Widget* createSvgRect3DEffectWidget(math::Rect rectMm, NVGcolor baseColor) {
	return createSvgRect3DEffectWidget(rectMm, baseColor, baseColor);
}

Widget* createSvgRect3DEffectWidget(math::Rect rectMm, NVGcolor baseColor, NVGcolor shadowBaseColor) {
	widget::FramebufferWidget* fb = new widget::FramebufferWidget();
	SvgRect3DEffectWidget* widget = new SvgRect3DEffectWidget();
	const float marginMm = 0.22f;
	widget->edgeMarginPx = mm2px(Vec(marginMm, 0.f)).x;
	widget->box.size = mm2px(rectMm.size.plus(Vec(2.f * marginMm, 2.f * marginMm)));
	widget->baseColor = baseColor;
	widget->shadowBaseColor = shadowBaseColor;
	fb->box.pos = mm2px(rectMm.pos.minus(Vec(marginMm, marginMm)));
	fb->box.size = widget->box.size;
	fb->dirtyOnSubpixelChange = false;
	fb->addChild(widget);
	return fb;
}

Widget* createPreviewFrameEnhancementWidget(math::Rect rectMm) {
	if (rectMm.size.x <= 0.f || rectMm.size.y <= 0.f) {
		return new Widget();
	}
	const float marginMm = 0.45f;
	widget::FramebufferWidget* fb = new widget::FramebufferWidget();
	fb->dirtyOnSubpixelChange = false;
	fb->box.pos = mm2px(rectMm.pos.minus(Vec(marginMm, marginMm)));
	fb->box.size = mm2px(rectMm.size.plus(Vec(2.f * marginMm, 2.f * marginMm)));

	PreviewFrameEnhancementWidget* frame = new PreviewFrameEnhancementWidget();
	frame->outsideMarginPx = mm2px(Vec(marginMm, 0.f)).x;
	frame->box.size = fb->box.size;
	fb->addChild(frame);
	return fb;
}

Widget* createPanelSurfaceEffectWidget(const std::string& svgPath, Vec panelSizePx) {
	if (svgPath.empty() || panelSizePx.x <= 0.f || panelSizePx.y <= 0.f) {
		Widget* empty = new Widget();
		empty->box.size = panelSizePx;
		return empty;
	}

	static std::map<std::string, PanelSurfaceEffectDefinition> cache;
	auto it = cache.find(svgPath);
	if (it == cache.end()) {
		it = cache.emplace(svgPath, loadPanelSurfaceEffectDefinition(svgPath)).first;
	}

	widget::FramebufferWidget* fb = new widget::FramebufferWidget();
	fb->dirtyOnSubpixelChange = false;
	fb->box.size = panelSizePx;

	PanelSurfaceEffectWidget* effect = new PanelSurfaceEffectWidget();
	effect->box.size = panelSizePx;
	effect->glassRects = it->second.glassRects;
	effect->screenRectsPx = it->second.screenRectsPx;
	fb->addChild(effect);
	return fb;
}

Widget* createLeviathanFooterLogoWidget(math::Rect boundsMm) {
	Widget* root = new Widget();
	root->box.pos = mm2px(boundsMm.pos);
	root->box.size = mm2px(boundsMm.size);

	AspectFitRasterImageWidget* full = new AspectFitRasterImageWidget("res/icon/leviathan_footer_full.png", 1.f);
	full->box.size = root->box.size;
	root->addChild(full);

	AspectFitRasterImageWidget* glow = new AspectFitRasterImageWidget("res/icon/leviathan_footer_glow.png", 1.f);
	glow->box.size = root->box.size;
	root->addChild(glow);

	return root;
}

int addSvgRect3DEffectWidgets(Widget* parent, const std::string& svgPath, const std::string& idSubstring) {
	if (!parent || svgPath.empty() || idSubstring.empty()) {
		return 0;
	}
	std::vector<panel_svg::SvgRectMatch> rects;
	if (!panel_svg::findRectsWithIdSubstringMm(svgPath, idSubstring, &rects)) {
		return 0;
	}
	int added = 0;
	for (const panel_svg::SvgRectMatch& match : rects) {
		if (match.hasFillColor && match.hasFillGradientEndColor) {
			parent->addChild(createSvgRect3DEffectWidget(match.rect, match.fillColor, match.fillGradientEndColor));
		}
		else if (match.hasFillColor) {
			parent->addChild(createSvgRect3DEffectWidget(match.rect, match.fillColor));
		}
		else {
			parent->addChild(createSvgRect3DEffectWidget(match.rect));
		}
		++added;
	}
	return added;
}

void resetEclipseShadowDrawMetrics() {
	gEclipseShadowDrawNs = 0u;
	gEclipseShadowDrawCount = 0u;
}

uint64_t eclipseShadowDrawNs() {
	return gEclipseShadowDrawNs;
}

uint64_t eclipseShadowDrawCount() {
	return gEclipseShadowDrawCount;
}

} // namespace visual_assets

namespace {

struct OrbScrewRasterLayer : TransparentWidget {
	std::string path;
	const float* rotationRad = nullptr;
	float imageSizePx = 0.f;

	explicit OrbScrewRasterLayer(std::string path)
		: path(std::move(path)) {
	}

	void draw(const DrawArgs& args) override {
		if (path.empty() || imageSizePx <= 0.f || box.size.x <= 0.f || box.size.y <= 0.f) {
			return;
		}
		const std::string fullPath = asset::plugin(pluginInstance, path);
		std::shared_ptr<window::Image> image = APP->window->loadImage(fullPath);
		if (!image || image->handle < 0) {
			return;
		}
		int imageHandle = visual_assets::loadPluginRasterMipmapHandle(args.vg, image, fullPath);
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
		float drawW = imageSizePx;
		float drawH = drawW / aspect;
		if (drawH > imageSizePx) {
			drawH = imageSizePx;
			drawW = drawH * aspect;
		}
		const Vec center = box.size.mult(0.5f);
		const float rotation = rotationRad ? *rotationRad : 0.f;

		nvgSave(args.vg);
		nvgTranslate(args.vg, center.x, center.y);
		if (std::fabs(rotation) > 1e-6f) {
			nvgRotate(args.vg, rotation);
		}
		NVGpaint paint = nvgImagePattern(
			args.vg,
			-0.5f * drawW,
			-0.5f * drawH,
			drawW,
			drawH,
			0.f,
			imageHandle,
			1.f);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, -0.5f * drawW, -0.5f * drawH, drawW, drawH);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
};

struct OrbScrewStaticLayer : TransparentWidget {
	std::string underlayPath;
	float imageSizePx = 0.f;

	void draw(const DrawArgs& args) override {
		if (imageSizePx <= 0.f || box.size.x <= 0.f || box.size.y <= 0.f) {
			return;
		}
		const Vec center = box.size.mult(0.5f);
		const float radius = 0.5f * imageSizePx;
		const Vec shadowCenter = center.plus(Vec(0.55f, 0.75f));

		nvgSave(args.vg);
		nvgTranslate(args.vg, shadowCenter.x, shadowCenter.y);
		nvgScale(args.vg, 1.f, 0.80f);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 0.f, 0.f, radius * 1.10f);
		nvgFillPaint(args.vg, nvgRadialGradient(
			args.vg,
			0.f,
			0.f,
			radius * 0.40f,
			radius * 1.10f,
			nvgRGBA(0, 0, 0, 155),
			nvgRGBA(0, 0, 0, 0)));
		nvgFill(args.vg);
		nvgRestore(args.vg);

		OrbScrewRasterLayer underlay(underlayPath);
		underlay.box.size = box.size;
		underlay.imageSizePx = imageSizePx;
		underlay.draw(args);
	}
};

void setSvgPortSizePx(app::SvgPort* port, float px, float rotationRad = 0.f) {
	if (!port) {
		return;
	}
	const Vec size(px, px);
	if (port->fb && port->sw) {
		const Vec svgSize = port->sw->box.size;
		const float svgMax = std::max(svgSize.x, svgSize.y);
		if (svgMax > 0.f) {
			const float scale = px / svgMax;
			port->fb->removeChild(port->sw);
			port->sw->box.pos = Vec(0.f, 0.f);
			TransformWidget* scaleTw = new TransformWidget();
			scaleTw->addChild(port->sw);
			scaleTw->scale(Vec(scale, scale));
			scaleTw->box.size = svgSize.mult(scale);
			if (std::fabs(rotationRad) > 1e-6f) {
				TransformWidget* rotateTw = new TransformWidget();
				rotateTw->addChild(scaleTw);
				rotateTw->rotate(rotationRad, size.div(2.f));
				rotateTw->box.size = size;
				port->fb->addChild(rotateTw);
			}
			else {
				port->fb->addChild(scaleTw);
			}
		}
	}
	port->box.size = size;
	if (port->fb) {
		port->fb->box.size = size;
	}
	if (port->shadow) {
		port->shadow->box.size = size;
	}
}

TransformWidget* setSvgSwitchSizePx(app::SvgSwitch* button, float px) {
	if (!button) {
		return nullptr;
	}
	TransformWidget* scaleTw = nullptr;
	const Vec size(px, px);
	if (button->fb && button->sw) {
		const Vec svgSize = button->sw->box.size;
		const float svgMax = std::max(svgSize.x, svgSize.y);
		if (svgMax > 0.f) {
			const float scale = px / svgMax;
			button->fb->removeChild(button->sw);
			button->sw->box.pos = Vec(0.f, 0.f);
			scaleTw = new TransformWidget();
			scaleTw->addChild(button->sw);
			scaleTw->scale(Vec(scale, scale));
			scaleTw->box.size = svgSize.mult(scale);
			button->fb->addChild(scaleTw);
		}
	}
	button->box.size = size;
	if (button->fb) {
		button->fb->box.size = size;
	}
	if (button->shadow) {
		button->shadow->box.size = size;
	}
	return scaleTw;
}

constexpr float kMagitekPortSizePx = 24.5f;
constexpr float kGoldButtonSizePx = 24.f;

struct MagitekInputShadow : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const Vec center = box.size.div(2.f).plus(Vec(1.6f, 2.5f));
		const float outerRadius = std::min(box.size.x, box.size.y) * 0.43f;
		const float innerRadius = outerRadius * 0.48f;
		NVGpaint paint = nvgRadialGradient(args.vg,
			center.x,
			center.y,
			innerRadius,
			outerRadius,
			nvgRGBA(0, 0, 0, 138),
			nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x, center.y, outerRadius);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}
};

struct MagitekOutputShadow : TransparentWidget {
	float rotationRad = 0.f;

	explicit MagitekOutputShadow(float rotationRad)
		: rotationRad(rotationRad) {
	}

	void drawHex(const DrawArgs& args, float radius, NVGcolor color) {
		const Vec center = box.size.div(2.f).plus(Vec(0.55f, 0.95f));
		nvgBeginPath(args.vg);
		for (int i = 0; i < 6; ++i) {
			const float angle = rotationRad - 0.5f * M_PI + float(i) * (M_PI / 3.f);
			const float x = center.x + std::cos(angle) * radius;
			const float y = center.y + std::sin(angle) * radius;
			if (i == 0) {
				nvgMoveTo(args.vg, x, y);
			}
			else {
				nvgLineTo(args.vg, x, y);
			}
		}
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
	}

	void draw(const DrawArgs& args) override {
		const float radius = kMagitekPortSizePx * 0.46f;
		drawHex(args, radius * 1.22f, nvgRGBA(0, 0, 0, 28));
		drawHex(args, radius * 1.04f, nvgRGBA(0, 0, 0, 62));
		drawHex(args, radius * 0.86f, nvgRGBA(0, 0, 0, 132));
	}
};

struct MagitekRasterImage : TransparentWidget {
	std::string path;
	const float* rotationRad = nullptr;

	explicit MagitekRasterImage(std::string path)
		: path(std::move(path)) {
	}

	int loadMipmapHandle(NVGcontext* vg, std::shared_ptr<window::Image> lifecycleImage, const std::string& fullPath) {
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

		int handle = nvgCreateImage(vg, fullPath.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
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

	void draw(const DrawArgs& args) override {
		if (path.empty()) {
			return;
		}
		const std::string fullPath = asset::plugin(pluginInstance, path);
		std::shared_ptr<window::Image> image = APP->window->loadImage(fullPath);
		if (!image || image->handle < 0) {
			return;
		}
		int imageHandle = loadMipmapHandle(args.vg, image, fullPath);
		if (imageHandle < 0) {
			imageHandle = image->handle;
		}
		const float rotation = rotationRad ? *rotationRad : 0.f;
		const Vec center = box.size.div(2.f);
		nvgSave(args.vg);
		if (std::fabs(rotation) > 1e-6f) {
			nvgTranslate(args.vg, center.x, center.y);
			nvgRotate(args.vg, rotation);
			NVGpaint paint = nvgImagePattern(args.vg, -center.x, -center.y, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
			nvgBeginPath(args.vg);
			nvgRect(args.vg, -center.x, -center.y, box.size.x, box.size.y);
			nvgFillPaint(args.vg, paint);
			nvgFill(args.vg);
		}
		else {
			NVGpaint paint = nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
			nvgFillPaint(args.vg, paint);
			nvgFill(args.vg);
		}
		nvgRestore(args.vg);
	}
};

static bool magitek2JackAnimationIsRotation(Magitek2JackAnimationStyle animationStyle) {
	return animationStyle == Magitek2JackAnimationStyle::CounterClockwiseRotation ||
		animationStyle == Magitek2JackAnimationStyle::ClockwiseRotation;
}

static bool magitek2JackAnimationIsRingPulse(Magitek2JackAnimationStyle animationStyle) {
	return animationStyle == Magitek2JackAnimationStyle::PurpleRingsInward ||
		animationStyle == Magitek2JackAnimationStyle::CyanRingsOutward;
}

constexpr float kMagitek2RingCycleSpacingSec = 0.62f;
constexpr float kMagitek2RingCycleLifeSec = 1.86f;
constexpr int kMagitek2RingCount = 3;

struct Magitek2RingPulseOverlay : TransparentWidget {
	const Magitek2RasterJack* jack = nullptr;

	explicit Magitek2RingPulseOverlay(const Magitek2RasterJack* jack)
		: jack(jack) {
	}

	void drawRing(const DrawArgs& args, const Vec& center, float radius, float alpha, NVGcolor color) {
		alpha = clamp(alpha, 0.f, 1.f);
		if (alpha <= 0.002f || radius <= 0.f) {
			return;
		}

		const unsigned char glowAlpha = (unsigned char) std::round(82.f * alpha);
		const unsigned char coreAlpha = (unsigned char) std::round(190.f * alpha);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x, center.y, radius);
		nvgStrokeWidth(args.vg, 1.45f);
		nvgStrokeColor(args.vg, nvgRGBA(color.r * 255.f, color.g * 255.f, color.b * 255.f, glowAlpha));
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x, center.y, radius);
		nvgStrokeWidth(args.vg, 0.52f);
		nvgStrokeColor(args.vg, nvgRGBA(color.r * 255.f, color.g * 255.f, color.b * 255.f, coreAlpha));
		nvgStroke(args.vg);
	}

	void draw(const DrawArgs& args) override {
		if (!jack || jack->ringOpacity <= 0.002f) {
			return;
		}

		const bool inward = jack->animationStyle == Magitek2JackAnimationStyle::PurpleRingsInward;
		if (!inward && jack->animationStyle != Magitek2JackAnimationStyle::CyanRingsOutward) {
			return;
		}

		const Vec center = box.size.div(2.f);
		const NVGcolor color = inward ? nvgRGB(0xa8, 0x62, 0xff) : nvgRGB(0x00, 0xc6, 0xe4);
		const float startRadius = inward ? 6.25f : 0.95f;
		const float endRadius = inward ? 0.95f : 6.3f;

		for (int i = 0; i < kMagitek2RingCount; ++i) {
			double localSec = jack->ringAnimationSec - double(i) * double(kMagitek2RingCycleSpacingSec);
			if (localSec < 0.0) {
				continue;
			}
			localSec = std::fmod(localSec, double(kMagitek2RingCycleLifeSec));
			const float t = clamp(float(localSec / double(kMagitek2RingCycleLifeSec)), 0.f, 1.f);
			const float smoothT = t * t * (3.f - 2.f * t);
			const float radius = crossfade(startRadius, endRadius, smoothT);
			const float fadeIn = clamp(t * 7.f, 0.f, 1.f);
			const float fadeOut = clamp((1.f - t) * 2.6f, 0.f, 1.f);
			const float alpha = jack->ringOpacity * fadeIn * fadeOut;
			drawRing(args, center, radius, alpha, color);
		}
	}
};

struct GoldButtonShadow : TransparentWidget {
	float pressAmount = 0.f;

	void draw(const DrawArgs& args) override {
		const float p = clamp(pressAmount, 0.f, 1.f);
		const Vec base = box.size.div(2.f);

		const Vec castCenter = base.plus(Vec(crossfade(1.15f, 0.25f, p), crossfade(2.55f, 1.35f, p)));
		const float castRx = box.size.x * crossfade(0.37f, 0.29f, p);
		const float castRy = box.size.y * crossfade(0.33f, 0.23f, p);
		NVGpaint castPaint = nvgRadialGradient(args.vg,
			castCenter.x,
			castCenter.y,
			box.size.x * crossfade(0.20f, 0.08f, p),
			box.size.x * crossfade(0.43f, 0.31f, p),
			nvgRGBA(0, 0, 0, int(std::round(crossfade(96.f, 50.f, p)))),
			nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgEllipse(args.vg, castCenter.x, castCenter.y, castRx, castRy);
		nvgFillPaint(args.vg, castPaint);
		nvgFill(args.vg);

		const Vec contactCenter = base.plus(Vec(crossfade(0.28f, 0.08f, p), crossfade(1.55f, 1.08f, p)));
		const float contactRx = box.size.x * crossfade(0.30f, 0.38f, p);
		const float contactRy = box.size.y * crossfade(0.11f, 0.085f, p);
		NVGpaint contactPaint = nvgRadialGradient(args.vg,
			contactCenter.x,
			contactCenter.y,
			box.size.x * crossfade(0.07f, 0.15f, p),
			box.size.x * crossfade(0.33f, 0.40f, p),
			nvgRGBA(0, 0, 0, int(std::round(crossfade(54.f, 128.f, p)))),
			nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgEllipse(args.vg, contactCenter.x, contactCenter.y, contactRx, contactRy);
		nvgFillPaint(args.vg, contactPaint);
		nvgFill(args.vg);
	}
};

struct GoldButtonFixedBezel : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const Vec c = box.size.div(2.f);
		const float r = box.size.x * 0.50f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c.x, c.y, r);
		nvgFillColor(args.vg, nvgRGB(17, 16, 19));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c.x, c.y, r - 0.7f);
		nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 190));
		nvgStrokeWidth(args.vg, 1.15f);
		nvgStroke(args.vg);
	}
};

struct GoldButtonPressOverlay : TransparentWidget {
	float pressAmount = 0.f;

	void draw(const DrawArgs& args) override {
		if (pressAmount <= 0.001f) {
			return;
		}
		const Vec c = box.size.div(2.f);
		const float radius = box.size.x * 0.47f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c.x, c.y, radius);
		NVGpaint shade = nvgLinearGradient(args.vg,
			c.x,
			c.y - radius,
			c.x,
			c.y + radius,
			nvgRGBA(0, 0, 0, int(72.f * pressAmount)),
			nvgRGBA(255, 238, 160, int(28.f * pressAmount)));
		nvgFillPaint(args.vg, shade);
		nvgFill(args.vg);
	}
};

void installMagitekShadow(app::SvgPort* port, Widget* customShadow) {
	if (!port || !customShadow) {
		delete customShadow;
		return;
	}
	if (port->shadow) {
		port->shadow->opacity = 0.f;
	}
	const bool inputShadow = dynamic_cast<MagitekInputShadow*>(customShadow) != nullptr;
	const bool outputShadow = dynamic_cast<MagitekOutputShadow*>(customShadow) != nullptr;
	widget::FramebufferWidget* shadowFb = new widget::FramebufferWidget();
	shadowFb->dirtyOnSubpixelChange = false;
	if (inputShadow) {
		const Vec bleed(8.f, 8.f);
		shadowFb->box.pos = bleed.mult(-0.5f);
		shadowFb->box.size = port->box.size.plus(bleed);
	}
	else if (outputShadow) {
		const Vec bleed(10.f, 10.f);
		shadowFb->box.pos = bleed.mult(-0.5f);
		shadowFb->box.size = port->box.size.plus(bleed);
	}
	else {
		shadowFb->box.size = port->box.size;
	}
	customShadow->box.pos = Vec(0.f, 0.f);
	customShadow->box.size = shadowFb->box.size;
	shadowFb->addChild(customShadow);
	if (port->fb) {
		port->addChildBelow(shadowFb, port->fb);
	}
	else {
		port->addChildBottom(shadowFb);
	}
}

struct ClockworkDragDebugRecorder {
	std::ofstream file;
	std::string path;
	double startTimeSec = 0.0;
	uint64_t sequence = 0;
	uint64_t gestureSequence = 0;

	std::string userRootPath() {
		return system::join(asset::user(), "Leviathan/UI");
	}

	bool ensureOpen() {
		if (file.is_open()) {
			return true;
		}
		system::createDirectories(userRootPath());
		const long long stampMs = (long long)std::llround(system::getUnixTime() * 1000.0);
		path = system::join(userRootPath(), "clockwork_knob_drag_" + std::to_string(stampMs) + ".csv");
		file.open(path);
		if (!file.is_open()) {
			WARN("Failed to open Clockwork knob drag debug CSV: %s", path.c_str());
			path.clear();
			return false;
		}
		file << std::setprecision(9);
		file << "sequence,gesture,t_sec,event,param_id,module_id,frame,knob_mode,mods,"
			<< "mouse_dx,mouse_dy,mouse_len,sent_dx,sent_dy,sent_len,max_len,clamped,value_before,value_after\n";
		startTimeSec = system::getTime();
		sequence = 0;
		DEBUG("Started Clockwork knob drag debug CSV: %s", path.c_str());
		return true;
	}

	uint64_t nextGesture() {
		return ++gestureSequence;
	}

	void log(
		const char* eventName,
		GearKnobInvertSized* knob,
		uint64_t gestureId,
		int frame,
		Vec mouseDelta,
		Vec sentDelta,
		float maxLen,
		bool clamped,
		float valueBefore,
		float valueAfter) {
		if (!ensureOpen()) {
			return;
		}
		const int moduleId = (knob && knob->module) ? knob->module->id : -1;
		const int knobMode = int(settings::knobMode);
		const int mods = (APP && APP->window) ? APP->window->getMods() : 0;
		const double tSec = std::max(0.0, system::getTime() - startTimeSec);
		file
			<< sequence++ << ','
			<< gestureId << ','
			<< tSec << ','
			<< (eventName ? eventName : "") << ','
			<< (knob ? knob->paramId : -1) << ','
			<< moduleId << ','
			<< frame << ','
			<< knobMode << ','
			<< mods << ','
			<< mouseDelta.x << ','
			<< mouseDelta.y << ','
			<< mouseDelta.norm() << ','
			<< sentDelta.x << ','
			<< sentDelta.y << ','
			<< sentDelta.norm() << ','
			<< maxLen << ','
			<< (clamped ? 1 : 0) << ','
			<< valueBefore << ','
			<< valueAfter << '\n';
		if (clamped || (eventName && eventName[0] == 'e')) {
			file.flush();
		}
	}
};

ClockworkDragDebugRecorder& clockworkDragDebugRecorder() {
	static ClockworkDragDebugRecorder recorder;
	return recorder;
}

float clockworkParamValue(GearKnobInvertSized* knob) {
	engine::ParamQuantity* pq = knob ? knob->getParamQuantity() : nullptr;
	return pq ? pq->getValue() : NAN;
}

static constexpr bool kClockworkLiquidShimmerEnabled = true;
static constexpr double kClockworkLiquidShimmerDurationSec = 0.70;

struct MovingSliderTeethRail : widget::Widget {
	app::SvgSlider* slider = nullptr;
	std::shared_ptr<window::Svg> railSvg;
	float drawWidthPx = 0.f;
	float drawHeightPx = 0.f;
	bool clipOpposingQuadrants = false;
	float centerTrackLeftPx = 0.f;
	float centerTrackRightPx = 0.f;
	float upperProtrusionTopPx = 0.f;
	float upperProtrusionBottomPx = 0.f;
	float lowerProtrusionTopPx = 0.f;
	float lowerProtrusionBottomPx = 0.f;

	void draw(const DrawArgs& args) override {
		if (!slider || !slider->handle || !railSvg || !railSvg->handle ||
				box.size.x <= 0.f || box.size.y <= 0.f ||
				drawWidthPx <= 0.f || drawHeightPx <= 0.f) {
			return;
		}

		const float topHandleY = std::min(slider->minHandlePos.y, slider->maxHandlePos.y);
		const float bottomHandleY = std::max(slider->minHandlePos.y, slider->maxHandlePos.y);
		const Vec svgSize = railSvg->getSize();
		if (svgSize.x <= 0.f || svgSize.y <= 0.f) {
			return;
		}

		// NanoVG translation is in screen pixels here. Center the artwork when
		// the handle is centered, then apply the handle's exact vertical pixel
		// displacement so the teeth and handle travel together at a 1:1 rate.
		const float handleCenterY = 0.5f * (topHandleY + bottomHandleY);
		const float handleOffsetY = slider->handle->box.pos.y - handleCenterY;
		const float svgScaleX = drawWidthPx / svgSize.x;
		const float svgScaleY = drawHeightPx / svgSize.y;
		const float railY = 0.5f * (box.size.y - drawHeightPx) + handleOffsetY;
		const float railX = 0.5f * (box.size.x - drawWidthPx);

		auto drawRail = [&](float clipX, float clipY, float clipWidth, float clipHeight) {
			if (clipWidth <= 0.f || clipHeight <= 0.f) {
				return;
			}
			nvgSave(args.vg);
			nvgIntersectScissor(args.vg, clipX, clipY, clipWidth, clipHeight);
			nvgTranslate(args.vg, railX, railY);
			nvgScale(args.vg, svgScaleX, svgScaleY);
			railSvg->draw(args.vg);
			nvgRestore(args.vg);
		};

		if (clipOpposingQuadrants &&
				centerTrackLeftPx > 0.f &&
				centerTrackRightPx > centerTrackLeftPx &&
				centerTrackRightPx < box.size.x &&
				upperProtrusionTopPx >= 0.f &&
				upperProtrusionBottomPx > upperProtrusionTopPx &&
				lowerProtrusionTopPx > upperProtrusionBottomPx &&
				lowerProtrusionBottomPx > lowerProtrusionTopPx &&
				lowerProtrusionBottomPx <= box.size.y) {
			// Draw the central housing once at full height. Splitting it into
			// two scissored passes creates a visible horizontal raster seam.
			drawRail(
				centerTrackLeftPx,
				0.f,
				centerTrackRightPx - centerTrackLeftPx,
				box.size.y
			);
			// Keep only the upper-right protrusions.
			drawRail(
				centerTrackRightPx,
				upperProtrusionTopPx,
				box.size.x - centerTrackRightPx,
				upperProtrusionBottomPx - upperProtrusionTopPx
			);
			// Keep only the lower-left protrusions.
			drawRail(
				0.f,
				lowerProtrusionTopPx,
				centerTrackLeftPx,
				lowerProtrusionBottomPx - lowerProtrusionTopPx
			);
		}
		else {
			drawRail(0.f, 0.f, box.size.x, box.size.y);
		}
	}
};

struct SliderRackGear : widget::Widget {
	app::SvgSlider* slider = nullptr;
	std::shared_ptr<window::Svg> gearSvg;
	std::shared_ptr<window::Svg> shadowSvg;
	float rotationDirection = 1.f;
	float pitchRadiusPx = 1.f;
	float restAngleRad = 0.f;

	void draw(const DrawArgs& args) override {
		if (!slider || !slider->handle || !gearSvg || !gearSvg->handle ||
				box.size.x <= 0.f || box.size.y <= 0.f || pitchRadiusPx <= 0.f) {
			return;
		}

		const Vec gearSvgSize = gearSvg->getSize();
		if (gearSvgSize.x <= 0.f || gearSvgSize.y <= 0.f) {
			return;
		}

		const float topHandleY = std::min(slider->minHandlePos.y, slider->maxHandlePos.y);
		const float bottomHandleY = std::max(slider->minHandlePos.y, slider->maxHandlePos.y);
		const float handleCenterY = 0.5f * (topHandleY + bottomHandleY);
		const float handleOffsetY = slider->handle->box.pos.y - handleCenterY;
		const float angleRad = restAngleRad + rotationDirection * handleOffsetY / pitchRadiusPx;
		auto drawSvg = [&] (
			const std::shared_ptr<window::Svg>& svg,
			Vec offset,
			float alpha,
			bool darkenAsShadow
		) {
			if (!svg || !svg->handle) {
				return;
			}
			const Vec svgSize = svg->getSize();
			if (svgSize.x <= 0.f || svgSize.y <= 0.f) {
				return;
			}
			const float scale = std::min(box.size.x / svgSize.x, box.size.y / svgSize.y);

			nvgSave(args.vg);
			nvgGlobalAlpha(args.vg, alpha);
			if (darkenAsShadow) {
				// Tint the exact metal SVG geometry black. Unlike destination-darkening
				// blending, this produces a visible source-over shadow in the cache.
				nvgGlobalTint(args.vg, nvgRGB(0x00, 0x00, 0x00));
			}
			nvgTranslate(
				args.vg,
				0.5f * box.size.x + offset.x,
				0.5f * box.size.y + offset.y
			);
			nvgRotate(args.vg, angleRad);
			nvgScale(args.vg, scale, scale);
			nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
			svg->draw(args.vg);
			nvgRestore(args.vg);
		};

		drawSvg(shadowSvg, Vec(0.4f, 0.45f), 0.6f, true);
		drawSvg(gearSvg, Vec(), 1.f, false);
	}
};

bool isInsideSliderControlArea(Vec pos, Vec widgetSize) {
	constexpr float controlWidthPx = 12.f;
	constexpr float controlHeightPx = 80.f;
	const math::Rect controlArea(
		Vec(
			0.5f * (widgetSize.x - controlWidthPx),
			0.5f * (widgetSize.y - controlHeightPx)
		),
		Vec(controlWidthPx, controlHeightPx)
	);
	return controlArea.contains(pos);
}

} // namespace

TorxScrew::TorxScrew() {
	box.size = Vec(RACK_GRID_WIDTH, RACK_GRID_WIDTH);
	fb = new widget::FramebufferWidget();
	fb->dirtyOnSubpixelChange = false;
	sw = new widget::SvgWidget();
	sw->setSvg(visual_assets::loadPluginSvgCached("res/icon/torx.svg"));
	fb->box.size = sw->box.size;
	fb->addChild(sw);
	addChild(fb);
}

void TorxScrew::draw(const DrawArgs& args) {
	if (!sw || sw->box.size.x <= 1.f || sw->box.size.y <= 1.f) return;
	const float scale = std::min(box.size.x / sw->box.size.x, box.size.y / sw->box.size.y);
	const Vec center = box.size.mult(0.5f);
	const Vec svgCenter = sw->box.size.mult(0.5f);

	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);
	nvgScale(args.vg, scale, scale);
	nvgTranslate(args.vg, -svgCenter.x, -svgCenter.y);
	Widget::draw(args);
	nvgRestore(args.vg);
}

HoverOrbScrew::HoverOrbScrew(const char* orbPath, const char* underlayPath, float spinDirection, NVGcolor glowColor) {
	constexpr float orbSizePx = 13.5f;
	box.size = Vec(RACK_GRID_WIDTH, RACK_GRID_WIDTH);
	this->spinDirection = spinDirection;

	auto* staticFb = new widget::FramebufferWidget;
	staticFb->dirtyOnSubpixelChange = false;
	staticFb->box.size = box.size;
	auto* staticLayer = new OrbScrewStaticLayer;
	staticLayer->underlayPath = underlayPath ? underlayPath : "";
	staticLayer->imageSizePx = orbSizePx;
	staticLayer->box.size = box.size;
	staticFb->addChild(staticLayer);
	addChild(staticFb);

	glowWidget = new GlowShimmerWidget;
	glowWidget->glowR = uint8_t(glowColor.r * 255.f);
	glowWidget->glowG = uint8_t(glowColor.g * 255.f);
	glowWidget->glowB = uint8_t(glowColor.b * 255.f);
	glowWidget->coreR = std::min(255, int(glowWidget->glowR) + 40);
	glowWidget->coreG = std::min(255, int(glowWidget->glowG) + 40);
	glowWidget->coreB = std::min(255, int(glowWidget->glowB) + 40);
	glowWidget->box.size = box.size;
	addChild(glowWidget);

	rotatingFb = new widget::FramebufferWidget;
	rotatingFb->dirtyOnSubpixelChange = false;
	rotatingFb->box.size = box.size;
	auto* orbLayer = new OrbScrewRasterLayer(orbPath ? orbPath : "");
	orbLayer->rotationRad = &rotationRad;
	orbLayer->imageSizePx = orbSizePx;
	orbLayer->box.size = box.size;
	rotatingFb->addChild(orbLayer);
	addChild(rotatingFb);

	lastSpinUpdateSec = system::getTime();
}

PurpleOrbScrew::PurpleOrbScrew()
	: HoverOrbScrew(
		"res/icon/purple_orb.png",
		"res/icon/purple_underlay.png",
		-1.f,
		nvgRGBA(0xa8, 0x62, 0xff, 0xff)) {
}

CyanOrbScrew::CyanOrbScrew()
	: HoverOrbScrew(
		"res/icon/cyan_orb.png",
		"res/icon/cyan_underlay.png",
		1.f,
		nvgRGBA(0xb8, 0x72, 0xff, 0xff)) {
	if (glowWidget) {
		glowWidget->coreR = 0xb8;
		glowWidget->coreG = 0x72;
		glowWidget->coreB = 0xff;
	}
}

void GlowShimmerWidget::draw(const DrawArgs& args) {
	if (opacity <= 1e-3f) {
		return;
	}

	const float radius = std::min(box.size.x, box.size.y) * 0.5f;
	const Vec center = box.size.mult(0.5f);

	const uint8_t r = glowR;
	const uint8_t g = glowG;
	const uint8_t b = glowB;

	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);

	const float alphaScale = pulse * opacity;

	// Outer bloom: deep glow filling most of the orb
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, 0.f, 0.f, radius * 0.85f);
	nvgFillPaint(args.vg, nvgRadialGradient(
		args.vg,
		0.f, 0.f,
		radius * 0.08f,
		radius * 0.85f,
		nvgRGBA(r, g, b, uint8_t(0x70 * alphaScale)),
		nvgRGBA(r, g, b, 0x00)));
	nvgFill(args.vg);

	// Mid glow: brighter color in the center half
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, 0.f, 0.f, radius * 0.50f);
	nvgFillPaint(args.vg, nvgRadialGradient(
		args.vg,
		0.f, 0.f,
		radius * 0.02f,
		radius * 0.50f,
		nvgRGBA(r, g, b, uint8_t(0x90 * alphaScale)),
		nvgRGBA(r, g, b, 0x00)));
	nvgFill(args.vg);

	// Hot core: very bright tinted-white center
	const uint8_t hotR = coreR;
	const uint8_t hotG = coreG;
	const uint8_t hotB = coreB;
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, 0.f, 0.f, radius * 0.18f);
	nvgFillPaint(args.vg, nvgRadialGradient(
		args.vg,
		0.f, 0.f,
		0.f,
		radius * 0.18f,
		nvgRGBA(hotR, hotG, hotB, uint8_t(0xaa * alphaScale)),
		nvgRGBA(r, g, b, uint8_t(0x40 * alphaScale))));
	nvgFill(args.vg);

	// Rotating highlight arc sweeping through the center
	const uint8_t arcAlpha = uint8_t(0x28 * alphaScale);
	nvgBeginPath(args.vg);
	nvgArc(args.vg, 0.f, 0.f, radius * 0.35f, shimmerPhaseRad - 0.8f, shimmerPhaseRad + 0.8f, NVG_CCW);
	nvgStrokeColor(args.vg, nvgRGBA(coreR, coreG, coreB, arcAlpha));
	nvgStrokeWidth(args.vg, radius * 0.04f);
	nvgStroke(args.vg);

	nvgRestore(args.vg);
}

void HoverOrbScrew::onEnter(const event::Enter& e) {
	hovered = true;
	if (glowWidget) {
		glowWidget->opacity = 1.f;
	}
	OpaqueWidget::onEnter(e);
}

void HoverOrbScrew::onLeave(const event::Leave& e) {
	hovered = false;
	OpaqueWidget::onLeave(e);
}

void HoverOrbScrew::step() {
	const double nowSec = system::getTime();
	const double dt = std::max(0.0, nowSec - lastSpinUpdateSec);
	lastSpinUpdateSec = nowSec;
	const float oldRotationRad = rotationRad;
	constexpr float hoverSpinRateRadPerSec = 0.333f;
	constexpr float returnRatePerSec = 19.7f;
	constexpr float glowFadeRatePerSec = 8.f;
	constexpr float shimmerRateRadPerSec = 1.35f;

	if (hovered) {
		rotationRad += float(dt * hoverSpinRateRadPerSec * spinDirection);
		if (std::fabs(rotationRad) > float(M_PI) * 2.f) {
			rotationRad = std::fmod(rotationRad, float(M_PI) * 2.f);
		}
	}
	else if (rotationRad != 0.f) {
		rotationRad *= std::exp(float(-dt * returnRatePerSec));
		if (std::fabs(rotationRad) < 1e-4f) {
			rotationRad = 0.f;
		}
	}

	if (glowWidget) {
		if (!hovered && glowWidget->opacity > 0.f) {
			glowWidget->opacity *= std::exp(float(-dt * glowFadeRatePerSec));
			if (glowWidget->opacity < 1e-3f) {
				glowWidget->opacity = 0.f;
			}
		}
		if (glowWidget->opacity > 0.f) {
			glowWidget->shimmerPhaseRad += float(dt * shimmerRateRadPerSec * spinDirection);
			if (std::fabs(glowWidget->shimmerPhaseRad) > float(M_PI) * 2.f) {
				glowWidget->shimmerPhaseRad = std::fmod(glowWidget->shimmerPhaseRad, float(M_PI) * 2.f);
			}
			glowWidget->pulse = 0.55f + 0.45f * std::sin(float(nowSec) * float(M_PI));
		}
	}

	if (rotatingFb && std::fabs(rotationRad - oldRotationRad) > 1e-6f) {
		rotatingFb->setDirty();
	}
	OpaqueWidget::step();
}

LeviathanSlider::LeviathanSlider() {
	constexpr float anchorWidthPx = 24.56693f;
	constexpr float anchorHeightPx = 98.26772f;
	constexpr float handleTravelInsetPx = 17.5f;
	constexpr float trackHeightPx = 80.f;
	constexpr float railClipYInTrackPx = 2.6426902f;
	constexpr float railClipHeightPx = 74.7691385f;
	constexpr float railArtworkWidthInViewBox = 9.8f;
	constexpr float railViewBoxWidth = 24.f;
	constexpr float railViewBoxHeight = 240.f;
	constexpr float railToothPitchInViewBox = 2.f;
	constexpr float railDrawHeightPx = 190.f;
	constexpr float railVisibleWidthPx = 4.5f;
	constexpr float railDrawWidthPx = railVisibleWidthPx * railViewBoxWidth / railArtworkWidthInViewBox;
	constexpr float gearSizePx = 10.5f;
	constexpr float gearToothCount = 20.f;
	constexpr float railToothPitchPx = railToothPitchInViewBox * railDrawHeightPx / railViewBoxHeight;
	constexpr float gearRotationSpeedTrim = 1.11f;
	constexpr float gearPitchRadiusPx =
		(railToothPitchPx * gearToothCount / (2.f * float(M_PI) * gearRotationSpeedTrim));
	constexpr float bottomGearPhaseOffsetRad = (float(M_PI) / gearToothCount) * -1.5;
	constexpr float leftGearCenterXPx = 5.8661845f;
	constexpr float rightGearCenterXPx = 18.7720951f;
	constexpr float topGearCenterYPx = 22.f;
	constexpr float bottomGearCenterYPx = anchorHeightPx - topGearCenterYPx;

	setBackgroundSvg(visual_assets::loadPluginSvgCached("res/icon/LeviathanSliderTrack.svg"));
	setHandleSvg(visual_assets::loadPluginSvgCached("res/icon/LeviathanSliderHandle.svg"));
	box.size = Vec(anchorWidthPx, anchorHeightPx);
	if (fb) {
		// SvgSlider keeps the complete mechanical assembly in this framebuffer.
		// Its onChange() invalidates the cache when the handle moves, so world
		// subpixel changes do not need to redraw the SVG layers.
		fb->dirtyOnSubpixelChange = false;
		fb->box.size = box.size;
	}
	using SliderLight = VCVSliderLight<LeviathanCyanPurpleLight>;
	auto* sliderLight = static_cast<SliderLight*>(light);
	if (sliderLight && sliderLight->fb) {
		sliderLight->fb->dirtyOnSubpixelChange = false;
	}
	if (background) {
		background->box.pos = Vec(
			0.5f * (anchorWidthPx - background->box.size.x),
			0.5f * (anchorHeightPx - background->box.size.y)
		);
	}
	if (fb && handle) {
		auto* teethRail = new MovingSliderTeethRail;
		teethRail->slider = this;
		teethRail->railSvg = visual_assets::loadPluginSvgCached("res/icon/dual_teeth_rounded_dark.svg");
		teethRail->box.pos = Vec(
			0.5f * (anchorWidthPx - railVisibleWidthPx),
			0.5f * (anchorHeightPx - trackHeightPx) + railClipYInTrackPx
		);
		teethRail->box.size = Vec(railVisibleWidthPx, railClipHeightPx);
		teethRail->drawWidthPx = railDrawWidthPx;
		teethRail->drawHeightPx = railDrawHeightPx;
		fb->addChildBelow(teethRail, handle);

		const std::shared_ptr<window::Svg> gearSvg =
			visual_assets::loadPluginSvgCached("res/icon/gear_metal.svg");
		auto addRackGear = [&](Vec center, float rotationDirection, float restAngleRad) {
			auto* gear = new SliderRackGear;
			gear->slider = this;
			gear->gearSvg = gearSvg;
			// Share the exact metal SVG as the shadow stencil for both gears.
			gear->shadowSvg = gearSvg;
			gear->rotationDirection = rotationDirection;
			gear->pitchRadiusPx = gearPitchRadiusPx;
			gear->restAngleRad = restAngleRad;
			gear->box.pos = center.minus(Vec(0.5f * gearSizePx, 0.5f * gearSizePx));
			gear->box.size = Vec(gearSizePx, gearSizePx);
			fb->addChildBelow(gear, handle);
		};
		addRackGear(
			Vec(leftGearCenterXPx, topGearCenterYPx),
			1.f,
			0.f
		);
		addRackGear(
			Vec(rightGearCenterXPx, bottomGearCenterYPx),
			-1.f,
			float(M_PI) + bottomGearPhaseOffsetRad
		);
	}
	setHandlePosCentered(
		math::Vec(anchorWidthPx * 0.5f, anchorHeightPx - handleTravelInsetPx),
		math::Vec(anchorWidthPx * 0.5f, handleTravelInsetPx)
	);
}

void LeviathanSlider::onHover(const event::Hover& e) {
	if (!isInsideSliderControlArea(e.pos, box.size)) {
		widget::Widget::onHover(e);
		return;
	}
	VCVLightSlider<LeviathanCyanPurpleLight>::onHover(e);
}

void LeviathanSlider::onHoverScroll(const event::HoverScroll& e) {
	if (!isInsideSliderControlArea(e.pos, box.size)) {
		widget::Widget::onHoverScroll(e);
		return;
	}
	VCVLightSlider<LeviathanCyanPurpleLight>::onHoverScroll(e);
}

void LeviathanSlider::onButton(const event::Button& e) {
	if (!isInsideSliderControlArea(e.pos, box.size)) {
		widget::Widget::onButton(e);
		return;
	}
	VCVLightSlider<LeviathanCyanPurpleLight>::onButton(e);
}

LuminSlider::LuminSlider() {
	constexpr float anchorWidthPx = 24.56693f;
	constexpr float anchorHeightPx = 98.26772f;
	constexpr float handleTravelInsetPx = 17.5f;
	constexpr float trackHeightPx = 80.f;
	constexpr float railClipYInTrackPx = 2.6426902f;
	constexpr float railClipHeightPx = 74.7691385f;
	constexpr float railViewBoxWidth = 24.f;
	constexpr float railViewBoxHeight = 240.f;
	constexpr float railDrawHeightPx = 190.f;
	constexpr float railDrawWidthPx = railDrawHeightPx * railViewBoxWidth / railViewBoxHeight;
	constexpr float railVisibleWidthPx = railDrawWidthPx;
	constexpr float fixedHousingLeftPx = 8.3191932f;
	constexpr float fixedHousingRightPx = 16.2477368f;
	constexpr float upperStaticTickOuterYPx = 12.826168f;
	constexpr float upperStaticTickInnerYPx = 45.099671f;
	constexpr float lowerStaticTickInnerYPx = 53.168049f;
	constexpr float lowerStaticTickOuterYPx = 85.441552f;

	setBackgroundSvg(visual_assets::loadPluginSvgCached("res/icon/LuminSliderTrack.svg"));
	setHandleSvg(visual_assets::loadPluginSvgCached("res/icon/LuminSliderHandle.svg"));
	box.size = Vec(anchorWidthPx, anchorHeightPx);
	if (fb) {
		// SvgSlider keeps the complete mechanical assembly in this framebuffer.
		// Its onChange() invalidates the cache when the handle moves, so world
		// subpixel changes do not need to redraw the SVG layers.
		fb->dirtyOnSubpixelChange = false;
		fb->box.size = box.size;
	}
	using SliderLight = VCVSliderLight<LeviathanCyanPurpleLight>;
	auto* sliderLight = static_cast<SliderLight*>(light);
	if (sliderLight && sliderLight->fb) {
		sliderLight->fb->dirtyOnSubpixelChange = false;
	}
	if (background) {
		background->box.pos = Vec(
			0.5f * (anchorWidthPx - background->box.size.x),
			0.5f * (anchorHeightPx - background->box.size.y)
		);
	}
	if (fb && background) {
		// The fixed housing does not need to be re-rasterized when the slider
		// value changes. Keep it in a nested framebuffer so the mechanical
		// framebuffer only composites its cached texture.
		fb->removeChild(background);
		auto* fixedBackgroundFb = new widget::FramebufferWidget;
		fixedBackgroundFb->dirtyOnSubpixelChange = false;
		fixedBackgroundFb->box.size = box.size;
		fixedBackgroundFb->addChild(background);
		fb->addChildBottom(fixedBackgroundFb);
	}
	if (fb && handle) {
		auto* teethRail = new MovingSliderTeethRail;
		teethRail->slider = this;
		teethRail->railSvg = visual_assets::loadPluginSvgCached("res/icon/dual_field_contact_track.svg");
		teethRail->box.pos = Vec(
			0.5f * (anchorWidthPx - railVisibleWidthPx),
			0.5f * (anchorHeightPx - trackHeightPx) + railClipYInTrackPx
		);
		teethRail->box.size = Vec(railVisibleWidthPx, railClipHeightPx);
		teethRail->drawWidthPx = railDrawWidthPx;
		teethRail->drawHeightPx = railDrawHeightPx;
		teethRail->clipOpposingQuadrants = true;
		teethRail->centerTrackLeftPx =
			fixedHousingLeftPx - teethRail->box.pos.x;
		teethRail->centerTrackRightPx =
			fixedHousingRightPx - teethRail->box.pos.x;
		teethRail->upperProtrusionTopPx =
			upperStaticTickOuterYPx - teethRail->box.pos.y;
		teethRail->upperProtrusionBottomPx =
			upperStaticTickInnerYPx - teethRail->box.pos.y;
		teethRail->lowerProtrusionTopPx =
			lowerStaticTickInnerYPx - teethRail->box.pos.y;
		teethRail->lowerProtrusionBottomPx =
			lowerStaticTickOuterYPx - teethRail->box.pos.y;
		fb->addChildBelow(teethRail, handle);
	}
	setHandlePosCentered(
		math::Vec(anchorWidthPx * 0.5f, anchorHeightPx - handleTravelInsetPx),
		math::Vec(anchorWidthPx * 0.5f, handleTravelInsetPx)
	);
}

void LuminSlider::onHover(const event::Hover& e) {
	if (!isInsideSliderControlArea(e.pos, box.size)) {
		widget::Widget::onHover(e);
		return;
	}
	VCVLightSlider<LeviathanCyanPurpleLight>::onHover(e);
}

void LuminSlider::onHoverScroll(const event::HoverScroll& e) {
	if (!isInsideSliderControlArea(e.pos, box.size)) {
		widget::Widget::onHoverScroll(e);
		return;
	}
	VCVLightSlider<LeviathanCyanPurpleLight>::onHoverScroll(e);
}

void LuminSlider::onButton(const event::Button& e) {
	if (!isInsideSliderControlArea(e.pos, box.size)) {
		widget::Widget::onButton(e);
		return;
	}
	VCVLightSlider<LeviathanCyanPurpleLight>::onButton(e);
}

MagitekInputJack::MagitekInputJack() {
	constexpr float rotationRad = float(M_PI) / 4.f;
	setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/icon/magitek_input.svg")));
	setSvgPortSizePx(this, kMagitekPortSizePx, rotationRad);
	installMagitekShadow(this, new MagitekInputShadow);
}

MagitekOutputJack::MagitekOutputJack() {
	constexpr float rotationRad = float(M_PI) / 6.f;
	setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/icon/magitek_output.svg")));
	setSvgPortSizePx(this, kMagitekPortSizePx, rotationRad);
	installMagitekShadow(this, new MagitekOutputShadow(rotationRad));
}

static float magitek2JackAnimationDirection(Magitek2JackAnimationStyle animationStyle) {
	switch (animationStyle) {
		case Magitek2JackAnimationStyle::CounterClockwiseRotation:
			return -1.f;
		case Magitek2JackAnimationStyle::ClockwiseRotation:
			return 1.f;
		case Magitek2JackAnimationStyle::None:
		default:
			return 0.f;
	}
}

Magitek2RasterJack::Magitek2RasterJack(const char* imagePath, Magitek2JackAnimationStyle animationStyle) {
	box.size = Vec(kMagitekPortSizePx, kMagitekPortSizePx);
	this->animationStyle = animationStyle;

	shadowFb = new widget::FramebufferWidget();
	shadowFb->dirtyOnSubpixelChange = false;
	const Vec bleed(8.f, 8.f);
	shadowFb->box.pos = bleed.mult(-0.5f);
	shadowFb->box.size = box.size.plus(bleed);
	MagitekInputShadow* shadow = new MagitekInputShadow;
	shadow->box.size = shadowFb->box.size;
	shadowFb->addChild(shadow);
	addChild(shadowFb);

	MagitekRasterImage* image = new MagitekRasterImage(imagePath ? imagePath : "");
	image->box.size = box.size;
	if (magitek2JackAnimationIsRotation(animationStyle)) {
		image->rotationRad = &hoverSpinRad;
	}
	addChild(image);

	if (magitek2JackAnimationIsRingPulse(animationStyle)) {
		Magitek2RingPulseOverlay* rings = new Magitek2RingPulseOverlay(this);
		rings->box.size = box.size;
		animationOverlay = rings;
		addChild(rings);
	}

	lastSpinUpdateSec = system::getTime();
}

Magitek2InputJack::Magitek2InputJack(Magitek2JackAnimationStyle animationStyle)
	: Magitek2RasterJack("res/icon/magitek2_input_rackfinal_256.png", animationStyle) {
}

Magitek2OutputJack::Magitek2OutputJack(Magitek2JackAnimationStyle animationStyle)
	: Magitek2RasterJack("res/icon/magitek2_output_rackfinal_256.png", animationStyle) {
}

void Magitek2RasterJack::onEnter(const event::Enter& e) {
	hovered = true;
	if (magitek2JackAnimationIsRingPulse(animationStyle)) {
		ringAnimationSec = 0.0;
	}
	PortWidget::onEnter(e);
}

void Magitek2RasterJack::onLeave(const event::Leave& e) {
	hovered = false;
	PortWidget::onLeave(e);
}

void Magitek2RasterJack::step() {
	const double nowSec = system::getTime();
	const double dt = std::max(0.0, nowSec - lastSpinUpdateSec);
	lastSpinUpdateSec = nowSec;
	constexpr float hoverSpinRateRadPerSec = 0.333f;
	const float spinDirection = magitek2JackAnimationDirection(animationStyle);

	engine::Port* port = getPort();
	const bool connected = port && port->isConnected();
	const bool hoverAnimating = hovered && !connected;
	if (hoverAnimating && spinDirection != 0.f) {
		hoverSpinRad += float(dt * hoverSpinRateRadPerSec * spinDirection);
		if (std::fabs(hoverSpinRad) > float(M_PI) * 2.f) {
			hoverSpinRad = std::fmod(hoverSpinRad, float(M_PI) * 2.f);
		}
	}
	else {
		hoverSpinRad *= 0.88f;
		if (std::fabs(hoverSpinRad) < 1e-4f) {
			hoverSpinRad = 0.f;
		}
	}

	if (magitek2JackAnimationIsRingPulse(animationStyle)) {
		const double startupSpeed = ringAnimationSec < double(kMagitek2RingCycleSpacingSec) ? 2.0 : 1.0;
		// Keep a continuous high-precision clock. Resetting this value to a
		// short phase re-enters the per-ring startup delays and causes a visible
		// pop even when the mathematical phase is otherwise close.
		ringAnimationSec += dt * startupSpeed;

		const float targetOpacity = hoverAnimating ? 1.f : 0.f;
		const float response = targetOpacity > ringOpacity ? 12.f : 8.f;
		const float amount = clamp(float(dt) * response, 0.f, 1.f);
		ringOpacity += (targetOpacity - ringOpacity) * amount;
		if (ringOpacity < 0.002f) {
			ringOpacity = 0.f;
		}
	}

	PortWidget::step();
}

GoldButton::GoldButton() {
	momentary = true;
	std::shared_ptr<window::Svg> svg = visual_assets::loadPluginSvgCached("res/icon/gold_button.svg");
	addFrame(svg);
	addFrame(svg);
	faceTransform = setSvgSwitchSizePx(this, kGoldButtonSizePx);
	if (shadow) {
		shadow->opacity = 0.f;
	}
	widget::FramebufferWidget* shadowLayerFb = new widget::FramebufferWidget();
	shadowLayerFb->dirtyOnSubpixelChange = false;
	shadowLayerFb->box.pos = Vec(-4.f, -3.f);
	shadowLayerFb->box.size = box.size.plus(Vec(8.f, 8.f));
	GoldButtonShadow* shadowWidget = new GoldButtonShadow();
	shadowWidget->box.size = shadowLayerFb->box.size;
	shadowLayerFb->addChild(shadowWidget);
	dropShadow = shadowWidget;
	dropShadowFb = shadowLayerFb;
	if (fb) {
		addChildBelow(shadowLayerFb, fb);
	}
	else {
		addChildBottom(shadowLayerFb);
	}

	widget::FramebufferWidget* bezelFb = new widget::FramebufferWidget();
	bezelFb->dirtyOnSubpixelChange = false;
	bezelFb->box.size = box.size;
	GoldButtonFixedBezel* bezel = new GoldButtonFixedBezel();
	bezel->box.size = bezelFb->box.size;
	bezelFb->addChild(bezel);
	fixedBezel = bezel;
	fixedBezelFb = bezelFb;
	if (fb) {
		addChildBelow(bezelFb, fb);
	}
	else {
		addChild(bezelFb);
	}

	widget::FramebufferWidget* overlayFb = new widget::FramebufferWidget();
	overlayFb->dirtyOnSubpixelChange = false;
	overlayFb->box.size = box.size;
	GoldButtonPressOverlay* overlay = new GoldButtonPressOverlay();
	overlay->box.size = overlayFb->box.size;
	overlayFb->addChild(overlay);
	pressOverlay = overlay;
	pressOverlayFb = overlayFb;
	addChild(overlayFb);
}

void GoldButton::step() {
	app::SvgSwitch::step();
	engine::ParamQuantity* pq = getParamQuantity();
	const float target = (pq && pq->getValue() > 0.5f) ? 1.f : 0.f;
	const float oldPressAmount = pressAmount;
	pressAmount += (target - pressAmount) * (target > pressAmount ? 0.34f : 0.42f);
	if (std::fabs(target - pressAmount) < 0.001f) {
		pressAmount = target;
	}
	const bool pressChanged = std::fabs(pressAmount - oldPressAmount) > 0.0001f;
	if (faceTransform) {
		faceTransform->identity();
		const float scale = kGoldButtonSizePx / 64.f;
		faceTransform->translate(Vec(0.f, 1.05f * pressAmount));
		faceTransform->scale(Vec(scale, scale));
	}
	if (auto* shadowWidget = dynamic_cast<GoldButtonShadow*>(dropShadow)) {
		shadowWidget->pressAmount = pressAmount;
	}
	if (auto* overlay = dynamic_cast<GoldButtonPressOverlay*>(pressOverlay)) {
		overlay->pressAmount = pressAmount;
	}
	if (dropShadowFb) {
		dropShadowFb->box.pos = Vec(-4.f, crossfade(-3.15f, -2.35f, pressAmount));
	}
	if (pressChanged) {
		if (fb) {
			fb->setDirty();
		}
		if (dropShadowFb) {
			dropShadowFb->setDirty();
		}
		if (pressOverlayFb) {
			pressOverlayFb->setDirty();
		}
	}
}

void GearKnobInvertSized::ActiveRingWidget::draw(const DrawArgs& args) {
	const float clampedValueNorm = clamp(valueNorm, 0.f, 1.f);
	const float clampedCenterNorm = clamp(centerNorm, 0.f, 1.f);
	const float knobAngle = crossfade(minAngle, maxAngle, clampedValueNorm);
	const float centerAngle = crossfade(minAngle, maxAngle, clampedCenterNorm);
	const float assetScale = sourceDiameterPx / sourceViewBoxPx;
	const Vec center(centerPx, centerPx);
	const float ringRadius = ringRadiusSourcePx * assetScale;
	const float ringWidth = ringWidthSourcePx * assetScale;
	const float activeRingWidth = activeRingWidthSourcePx * assetScale;
	const float startAngle = -0.5f * M_PI + minAngle;
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float activeAngle = -0.5f * M_PI + knobAngle;
	const float centerArcAngle = -0.5f * M_PI + centerAngle;

	nvgSave(args.vg);

	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, ringRadius, startAngle, endAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(2, 1, 1, 230));
	nvgStrokeWidth(args.vg, ringWidth);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	const bool drawActive = !bipolar || std::fabs(clampedValueNorm - clampedCenterNorm) > 0.001f;
	if (drawActive) {
		const float activeStartAngle = bipolar ? centerArcAngle : startAngle;
		const float activeEndAngle = activeAngle;
		nvgBeginPath(args.vg);
		nvgArc(args.vg,
			center.x,
			center.y,
			ringRadius,
			std::min(activeStartAngle, activeEndAngle),
			std::max(activeStartAngle, activeEndAngle),
			NVG_CW);
		NVGpaint activePaint = nvgLinearGradient(args.vg,
			center.x - ringRadius, center.y,
			center.x + ringRadius, center.y,
			// Eclipse-orange option: nvgRGBA(240, 138, 36, 248), nvgRGBA(255, 210, 154, 255)
			nvgRGBA(255, 218, 42, 248),
			nvgRGBA(255, 250, 205, 255));
		nvgStrokePaint(args.vg, activePaint);
		nvgStrokeWidth(args.vg, activeRingWidth);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		if (kClockworkLiquidShimmerEnabled) {
			const double now = system::getTime();
			const double remaining = liquidShimmerUntil - now;
			if (remaining > 0.0) {
				const float fade = clamp(float(remaining / kClockworkLiquidShimmerDurationSec), 0.f, 1.f);
				const float phase = float(std::fmod(now * 1.35, 1.0));
				const float sweepX = ringRadius * 2.4f;
				const float shimmerStartX = center.x - ringRadius * 1.2f + sweepX * phase;
				NVGpaint shimmerPaint = nvgLinearGradient(args.vg,
					shimmerStartX, center.y - ringRadius,
					shimmerStartX + ringRadius * 0.55f, center.y + ringRadius,
					// Eclipse-orange option: nvgRGBA(255, 215, 163, 0), nvgRGBA(255, 230, 190, alpha)
					nvgRGBA(255, 255, 220, 0),
					nvgRGBA(255, 255, 250, (unsigned char) std::round(118.f * fade)));
				nvgBeginPath(args.vg);
				nvgArc(args.vg,
					center.x,
					center.y,
					ringRadius,
					std::min(activeStartAngle, activeEndAngle),
					std::max(activeStartAngle, activeEndAngle),
					NVG_CW);
				nvgStrokePaint(args.vg, shimmerPaint);
				nvgStrokeWidth(args.vg, std::max(1.f, activeRingWidth * 0.55f));
				nvgLineCap(args.vg, NVG_ROUND);
				nvgStroke(args.vg);
			}
		}
	}

	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, ringRadius - 0.5f * ringWidth, startAngle, endAngle, NVG_CW);
	// Eclipse-orange option: nvgRGBA(255, 210, 154, 76)
	nvgStrokeColor(args.vg, nvgRGBA(255, 244, 154, 80));
	if (innerLineWidthSourcePx > 0.f) {
		nvgStrokeWidth(args.vg, innerLineWidthSourcePx * assetScale);
		nvgStroke(args.vg);
	}

	nvgRestore(args.vg);
}

GearKnobInvertSized::ShadowWidget::ShadowWidget() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
}

void GearKnobInvertSized::ShadowWidget::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void GearKnobInvertSized::ShadowWidget::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 1.f) return;

	const float angle = crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float scale = diameterPx / std::max(svgSize.x, svgSize.y);
	const Vec center = box.size.mult(0.5f);
	struct ShadowPass {
		float offsetX;
		float offsetY;
		float scaleMul;
		float alpha;
	};
	const ShadowPass passes[] = {
		{0.18f, 0.28f, 1.003f, 62.f / 255.f},
		{0.45f, 0.62f, 1.010f, 106.f / 255.f},
		{0.78f, 1.05f, 1.020f, 58.f / 255.f},
	};

	for (const ShadowPass& pass : passes) {
		nvgSave(args.vg);
		nvgGlobalAlpha(args.vg, pass.alpha);
		nvgTranslate(args.vg, center.x + pass.offsetX, center.y + pass.offsetY);
		nvgRotate(args.vg, angle);
		nvgScale(args.vg, scale * pass.scaleMul, scale * pass.scaleMul);
		nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
		Widget::draw(args);
		nvgRestore(args.vg);
	}
}

GearKnobInvertSized::GearKnobInvertSized() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	setCachedSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_invert.svg"));
	if (shadow) {
		shadow->opacity = 0.f;
	}
	shadowLayer = new ShadowWidget();
	shadowLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_shadow.svg"));
	shadowLayer->box.size = box.size;
	shadowLayer->minAngle = minAngle;
	shadowLayer->maxAngle = maxAngle;
	shadowLayer->valueNorm = normalizedParamValue();
	fb->addChildBelow(shadowLayer, tw);
	activeRing = new ActiveRingWidget();
	activeRing->box.size = box.size;
	activeRing->minAngle = minAngle;
	activeRing->maxAngle = maxAngle;
	activeRing->valueNorm = normalizedParamValue();
	fb->addChild(activeRing);
}

void GearKnobInvertSized::draw(const DrawArgs& args) {
	app::SvgKnob::draw(args);
}

void GearKnobInvertSized::step() {
	app::SvgKnob::step();
	if (kClockworkLiquidShimmerEnabled && activeRing && fb && system::getTime() < activeRing->liquidShimmerUntil) {
		fb->setDirty();
	}
}

void GearKnobInvertSized::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (shadowLayer) {
		shadowLayer->valueNorm = valueNorm;
	}
	if (activeRing) {
		activeRing->valueNorm = valueNorm;
		if (kClockworkLiquidShimmerEnabled) {
			activeRing->liquidShimmerUntil = system::getTime() + kClockworkLiquidShimmerDurationSec;
		}
	}
	if (fb) {
		fb->setDirty();
	}
}

void GearKnobInvertSized::onDragStart(const DragStartEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		app::SvgKnob::onDragStart(e);
		return;
	}
	dragMoveFrame = 0;
	if (isClockworkDragDebugLoggingEnabled()) {
		dragLogGestureId = clockworkDragDebugRecorder().nextGesture();
		const float valueBefore = clockworkParamValue(this);
		clockworkDragDebugRecorder().log("start", this, dragLogGestureId, -1, Vec(), Vec(), 0.f, false, valueBefore, valueBefore);
	}
	else {
		dragLogGestureId = 0;
	}
	app::SvgKnob::onDragStart(e);
}

void GearKnobInvertSized::onDragEnd(const DragEndEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		app::SvgKnob::onDragEnd(e);
		return;
	}
	if (isClockworkDragDebugLoggingEnabled() && dragLogGestureId != 0) {
		const float valueAfter = clockworkParamValue(this);
		clockworkDragDebugRecorder().log("end", this, dragLogGestureId, dragMoveFrame, Vec(), Vec(), 0.f, false, valueAfter, valueAfter);
	}
	dragMoveFrame = 0;
	dragLogGestureId = 0;
	app::SvgKnob::onDragEnd(e);
}

void GearKnobInvertSized::onDragMove(const DragMoveEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		app::SvgKnob::onDragMove(e);
		return;
	}
	const bool logMove = isClockworkDragDebugLoggingEnabled() && dragLogGestureId != 0 && dragMoveFrame < 8;
	const float valueBefore = logMove ? clockworkParamValue(this) : NAN;
	dragMoveFrame++;
	app::SvgKnob::onDragMove(e);
	if (logMove) {
		const float valueAfter = clockworkParamValue(this);
		clockworkDragDebugRecorder().log("move", this, dragLogGestureId, dragMoveFrame - 1, e.mouseDelta, e.mouseDelta, 0.f, false, valueBefore, valueAfter);
	}
}

void GearKnobInvertSized::setCachedSvg(std::shared_ptr<window::Svg> svg) {
	app::SvgKnob::setSvg(svg);
	if (sw) {
		sw->hide();
	}
	if (!svg) {
		return;
	}
	if (!cachedSvgFb) {
		cachedSvgFb = new widget::FramebufferWidget();
		cachedSvgFb->dirtyOnSubpixelChange = false;
		cachedSvgSw = new widget::SvgWidget();
		cachedSvgFb->addChild(cachedSvgSw);
		tw->addChild(cachedSvgFb);
	}
	if (cachedSvgSw) {
		cachedSvgSw->setSvg(svg);
		cachedSvgFb->box.size = cachedSvgSw->box.size;
		cachedSvgFb->setDirty();
	}
	if (shadowLayer) {
		shadowLayer->box.size = box.size;
	}
}

float GearKnobInvertSized::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

TinyClockworkGearKnob::TinyClockworkGearKnob() {
	minAngle = -0.8 * M_PI;
	maxAngle = 0.8 * M_PI;
	setCachedSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_tiny.svg"));
	if (shadowLayer) {
		shadowLayer->box.size = box.size;
		shadowLayer->minAngle = minAngle;
		shadowLayer->maxAngle = maxAngle;
		shadowLayer->valueNorm = normalizedParamValue();
	}
	if (activeRing) {
		activeRing->box.size = box.size;
		activeRing->minAngle = minAngle;
		activeRing->maxAngle = maxAngle;
		activeRing->valueNorm = normalizedParamValue();
		activeRing->centerPx = 12.f;
		activeRing->sourceDiameterPx = 24.f;
		activeRing->sourceViewBoxPx = 56.f;
		activeRing->ringRadiusSourcePx = 16.4f;
		activeRing->ringWidthSourcePx = 8.0f;
		activeRing->activeRingWidthSourcePx = 5.8f;
		activeRing->innerLineWidthSourcePx = 0.0f;
	}
	if (fb) {
		fb->setDirty();
	}
}

BipolarTinyClockworkGearKnob::BipolarTinyClockworkGearKnob() {
	if (activeRing) {
		activeRing->bipolar = true;
		activeRing->centerNorm = 0.5f;
		activeRing->valueNorm = normalizedParamValue();
	}
	if (fb) {
		fb->setDirty();
	}
}

void EclipseKnob::ProgressRingWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float radiusPx = diameterPx * (46.f / 120.f);
	const float strokeWidthPx = std::max(1.35f, diameterPx * (5.8f / 120.f));
	const float inactiveStrokeWidthPx = std::max(0.95f, strokeWidthPx * 0.84f);
	const float inactiveRadiusPx = radiusPx - 0.5f * (inactiveStrokeWidthPx - strokeWidthPx * 0.72f);
	const float activeStrokeWidthPx = std::max(strokeWidthPx, diameterPx * (7.2f / 120.f));
	const float activeRadiusPx = radiusPx - 0.5f * (activeStrokeWidthPx - strokeWidthPx);
	const float startNorm = bipolar ? centerNorm : 0.f;
	const float startAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(startNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float sweep = std::fabs(endAngle - startAngle);
	const float dashAngle = 0.11f;
	const float gapAngle = 0.225f;
	const float periodAngle = dashAngle + gapAngle;
	const float topAngle = -0.5f * float(M_PI);

	nvgSave(args.vg);
	nvgLineCap(args.vg, NVG_ROUND);

	const float ringMinAngle = -0.5f * float(M_PI) + minAngle;
	const float ringMaxAngle = -0.5f * float(M_PI) + maxAngle;
	float firstSegmentAngle = topAngle + 0.5f * gapAngle;
	while (firstSegmentAngle - periodAngle > ringMinAngle) {
		firstSegmentAngle -= periodAngle;
	}
	for (float a = firstSegmentAngle; a < ringMaxAngle; a += periodAngle) {
		const float b = std::min(a + dashAngle, ringMaxAngle);
		if (b <= ringMinAngle) continue;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, inactiveRadiusPx, std::max(a, ringMinAngle), b, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(142, 124, 72, 118));
		nvgStrokeWidth(args.vg, inactiveStrokeWidthPx);
		nvgStroke(args.vg);
	}

	if (sweep > 0.008f) {
		const NVGcolor activeColor = nvgRGBA(255, 242, 184, 248);
		const float activeMinAngle = std::min(startAngle, endAngle);
		const float activeMaxAngle = std::max(startAngle, endAngle);
		for (float a = firstSegmentAngle; a < activeMaxAngle; a += periodAngle) {
			const float b = a + dashAngle;
			const float a0 = std::max(a, activeMinAngle);
			const float a1 = std::min(b, activeMaxAngle);
			if (a1 <= a0) continue;
			nvgBeginPath(args.vg);
			nvgArc(args.vg,
				center.x,
				center.y,
				activeRadiusPx,
				a0,
				a1,
				NVG_CW);
			nvgStrokeColor(args.vg, activeColor);
			nvgStrokeWidth(args.vg, activeStrokeWidthPx);
			nvgStroke(args.vg);
		}
	}

	nvgRestore(args.vg);
}

EclipseKnob::SvgLayer::SvgLayer() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
	scaleFactor = 1.0f;
}

void EclipseKnob::SvgLayer::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void EclipseKnob::SvgLayer::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 1.f) return;

	const float scale = (diameterPx / std::max(svgSize.x, svgSize.y)) * scaleFactor;
	const Vec center = box.size.mult(0.5f);
	const float angle = rotateWithValue ? crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f)) : 0.f;

	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);
	nvgRotate(args.vg, angle);
	nvgScale(args.vg, scale, scale);
	nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
	Widget::draw(args);
	nvgRestore(args.vg);
}

EclipseKnob::ShadowWidget::ShadowWidget() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
	scaleFactor = 1.0f;
}

void EclipseKnob::ShadowWidget::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void EclipseKnob::ShadowWidget::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 1.f) return;

	const bool measure = isDragonKingDebugEnabled();
	const std::chrono::steady_clock::time_point start = measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
	const float angle = crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float scale = (diameterPx / std::max(svgSize.x, svgSize.y)) * scaleFactor;
	const Vec center = box.size.mult(0.5f);
	struct ShadowPass {
		float offsetX;
		float offsetY;
		float scaleMul;
		float alpha;
	};
	const ShadowPass passes[] = {
		{0.22f, 0.32f, 1.003f, 62.f / 255.f},
		{0.52f, 0.70f, 1.009f, 106.f / 255.f},
		{0.90f, 1.18f, 1.018f, 58.f / 255.f},
	};

	for (const ShadowPass& pass : passes) {
		nvgSave(args.vg);
		nvgGlobalAlpha(args.vg, pass.alpha);
		nvgTranslate(args.vg, center.x + pass.offsetX * scaleFactor, center.y + pass.offsetY * scaleFactor);
		nvgRotate(args.vg, angle);
		nvgScale(args.vg, scale * pass.scaleMul, scale * pass.scaleMul);
		nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
		Widget::draw(args);
		nvgRestore(args.vg);
	}
	if (measure) {
		visual_assets::gEclipseShadowDrawNs += uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - start).count());
		visual_assets::gEclipseShadowDrawCount++;
	}
}

EclipseKnob::EclipseKnob() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/EclipseKnobBack.svg");
	app::SvgKnob::setSvg(backSvg);
	box.size = Vec(28.f, 28.f);
	if (fb) {
		fb->box.size = box.size;
	}
	if (sw) {
		sw->hide();
	}
	if (shadow) {
		shadow->opacity = 0.f;
	}
	shadowLayer = new ShadowWidget();
	shadowLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/EclipseKnobShadow.svg"));
	shadowLayer->box.size = box.size;
	shadowLayer->minAngle = minAngle;
	shadowLayer->maxAngle = maxAngle;
	shadowLayer->valueNorm = normalizedParamValue();
	fb->addChild(shadowLayer);
	setBackSvg(backSvg);
	progressRing = new ProgressRingWidget();
	progressRing->box.size = box.size;
	progressRing->minAngle = minAngle;
	progressRing->maxAngle = maxAngle;
	progressRing->valueNorm = normalizedParamValue();
	fb->addChild(progressRing);
	setPointerSvg(visual_assets::loadPluginSvgCached("res/icon/EclipseKnobPointer.svg"));
}

void EclipseKnob::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (shadowLayer) {
		shadowLayer->valueNorm = valueNorm;
	}
	if (backLayer) {
		backLayer->valueNorm = valueNorm;
	}
	if (pointerLayer) {
		pointerLayer->valueNorm = valueNorm;
	}
	if (progressRing) {
		progressRing->valueNorm = valueNorm;
	}
	if (fb) {
		fb->setDirty();
	}
}

void EclipseKnob::setBackSvg(std::shared_ptr<window::Svg> svg) {
	if (!svg || !fb) return;
	if (!backLayer) {
		backLayer = new SvgLayer();
		backLayer->minAngle = minAngle;
		backLayer->maxAngle = maxAngle;
		backLayer->valueNorm = normalizedParamValue();
		backLayer->rotateWithValue = true;
		fb->addChild(backLayer);
	}
	backLayer->setSvg(svg);
	backLayer->box.size = box.size;
}

void EclipseKnob::setPointerSvg(std::shared_ptr<window::Svg> svg) {
	if (!svg || !fb) return;
	if (!pointerLayer) {
		pointerLayer = new SvgLayer();
		pointerLayer->minAngle = minAngle;
		pointerLayer->maxAngle = maxAngle;
		pointerLayer->valueNorm = normalizedParamValue();
		pointerLayer->rotateWithValue = true;
		fb->addChild(pointerLayer);
	}
	pointerLayer->setSvg(svg);
	pointerLayer->box.size = box.size;
}

float EclipseKnob::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

void EclipseKnob::setProgressRingBipolar(bool bipolar, float centerNorm) {
	if (!progressRing) return;
	progressRing->bipolar = bipolar;
	progressRing->centerNorm = clamp(centerNorm, 0.f, 1.f);
	if (fb) {
		fb->setDirty();
	}
}

void Eclipse2Knob::ProgressLedRingWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	// LEDs are positioned slightly outside the bezel with a clean gap, scaled to fit inside bounds
	const float radiusPx = diameterPx * (45.0f / 120.f);
	const float largeRadiusPx = std::max(0.48f, diameterPx * (1.8f / 120.f));

	const float startNorm = bipolar ? centerNorm : 0.f;
	const float minLitNorm = std::min(startNorm, valueNorm);
	const float maxLitNorm = std::max(startNorm, valueNorm);
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.0f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);
	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	nvgSave(args.vg);

	// 1. Draw Recessed Dark Track Ring
	const float startArcAngle = minAngle - 0.5f * M_PI;
	const float endArcAngle = maxAngle - 0.5f * M_PI;

	// Subtle outer drop shadow for track depth
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(3, 2, 2, 96));
	nvgStrokeWidth(args.vg, largeRadiusPx * 4.4f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	// Sharp black border stroke around the track (thickened and slightly transparent)
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 245));
	nvgStrokeWidth(args.vg, largeRadiusPx * 3.4f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	// Core dark track channel
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(14, 12, 11, 230));
	nvgStrokeWidth(args.vg, largeRadiusPx * 2.4f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	// Light specular accent inside the track (warm bronze glint)
	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, radiusPx, startArcAngle, endArcAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(255, 220, 150, 16));
	nvgStrokeWidth(args.vg, largeRadiusPx * 1.8f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	// Active track background glow (linking the active LEDs together)
	const float activeStartAngle = (minAngle + minLitNorm * (maxAngle - minAngle)) - 0.5f * M_PI;
	const float activeEndAngle = (minAngle + maxLitNorm * (maxAngle - minAngle)) - 0.5f * M_PI;
	const float activeSweep = activeEndAngle - activeStartAngle;
	if (activeSweep > 0.008f && bloom > 0.001f) {
		// Wide soft glow bloom
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, activeStartAngle, activeEndAngle, NVG_CW);
		nvgStrokeColor(args.vg, bloomColor(nvgRGBA(255, 175, 40, 36)));
		nvgStrokeWidth(args.vg, largeRadiusPx * 6.5f);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		// Tighter core glow bloom
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, activeStartAngle, activeEndAngle, NVG_CW);
		nvgStrokeColor(args.vg, bloomColor(nvgRGBA(255, 215, 95, 76)));
		nvgStrokeWidth(args.vg, largeRadiusPx * 4.2f);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);
	}

	for (int i = 0; i < numLeds; ++i) {
		const float ledNorm = float(i) / float(numLeds - 1);
		const float angle = minAngle + ledNorm * (maxAngle - minAngle);

		const float x = center.x + radiusPx * std::sin(angle);
		const float y = center.y - radiusPx * std::cos(angle);

		const float r = largeRadiusPx;

		bool active = false;
		if (bipolar) {
			active = (ledNorm >= minLitNorm && ledNorm <= maxLitNorm) && (std::fabs(valueNorm - centerNorm) > 0.005f);
		}
		else {
			active = (valueNorm > 0.f) && (ledNorm <= valueNorm);
		}

		if (active) {
			const float litR = r * 0.88f;

			if (bloom > 0.001f) {
				// Glow aura (focused, brighter and more opaque, warm gold-orange)
				NVGpaint glow = nvgRadialGradient(
					args.vg,
					x, y,
					litR * 0.4f,
					litR * 3.2f,
					bloomColor(nvgRGBA(255, 235, 140, 255)),
					nvgRGBA(255, 110, 10, 0)
				);
				nvgBeginPath(args.vg);
				nvgCircle(args.vg, x, y, litR * 3.2f);
				nvgFillPaint(args.vg, glow);
				nvgFill(args.vg);
			}

			// Core LED dot (brighter warm gold-cream)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, litR);
			nvgFillColor(args.vg, nvgRGBA(255, 252, 200, 255));
			nvgFill(args.vg);

			// Intense central light source hotspot (pure white)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, litR * 0.55f);
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
			nvgFill(args.vg);

			// Edge accent (matching active stroke, bright warm orange)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, litR);
			nvgStrokeColor(args.vg, nvgRGBA(255, 200, 50, 245));
			nvgStrokeWidth(args.vg, std::max(0.35f, diameterPx * (0.4f / 120.f)));
			nvgStroke(args.vg);
		}
		else {
			// Inactive dot (matching EclipseKnob inactive)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, r);
			nvgFillColor(args.vg, nvgRGBA(142, 124, 72, 118));
			nvgFill(args.vg);

			// Inactive outline (subtle warm gold-bronze border)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, r);
			nvgStrokeColor(args.vg, nvgRGBA(80, 70, 40, 96));
			nvgStrokeWidth(args.vg, std::max(0.3f, diameterPx * (0.3f / 120.f)));
			nvgStroke(args.vg);
		}
	}

	nvgRestore(args.vg);
}

void Eclipse2Knob::ShadowWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	// Knob visual radius at 0.70f scale factor:
	const float knobRadius = diameterPx * 0.315f;

	nvgSave(args.vg);

	// Multi-pass soft diffused radial gradient shadows for realistic depth:
	// Pass 1: Wide, very soft ambient shadow (ambient occlusion)
	{
		const float offsetX = diameterPx * (0.4f / 34.f);
		const float offsetY = diameterPx * (0.8f / 34.f);
		const float blurRadius = knobRadius * 1.5f;
		NVGpaint shadowPaint = nvgRadialGradient(
			args.vg,
			center.x + offsetX,
			center.y + offsetY,
			knobRadius * 0.5f,
			blurRadius,
			nvgRGBA(0, 0, 0, 80), // soft black center
			nvgRGBA(0, 0, 0, 0)   // fading to transparent
		);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x + offsetX, center.y + offsetY, blurRadius);
		nvgFillPaint(args.vg, shadowPaint);
		nvgFill(args.vg);
	}

	// Pass 2: Tighter, slightly darker contact shadow
	{
		const float offsetX = diameterPx * (0.25f / 34.f);
		const float offsetY = diameterPx * (0.5f / 34.f);
		const float blurRadius = knobRadius * 1.15f;
		NVGpaint shadowPaint = nvgRadialGradient(
			args.vg,
			center.x + offsetX,
			center.y + offsetY,
			knobRadius * 0.8f,
			blurRadius,
			nvgRGBA(0, 0, 0, 136), // black center
			nvgRGBA(0, 0, 0, 0)
		);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x + offsetX, center.y + offsetY, blurRadius);
		nvgFillPaint(args.vg, shadowPaint);
		nvgFill(args.vg);
	}

	nvgRestore(args.vg);
}

Eclipse2Knob::Eclipse2Knob() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/Eclipse2Knob.svg");
	app::SvgKnob::setSvg(backSvg);
	box.size = Vec(34.f, 34.f);
	if (fb) {
		fb->box.size = box.size;
	}
	if (sw) {
		sw->hide();
	}
	if (shadow) {
		shadow->opacity = 0.f;
	}
	lastBloomAmount = settings::haloBrightness;

	shadowLayer = new ShadowWidget();
	shadowLayer->box.size = box.size;
	shadowLayer->minAngle = minAngle;
	shadowLayer->maxAngle = maxAngle;
	shadowLayer->valueNorm = normalizedParamValue();
	fb->addChild(shadowLayer);

	progressRing = new ProgressLedRingWidget();
	progressRing->box.size = box.size;
	progressRing->minAngle = minAngle;
	progressRing->maxAngle = maxAngle;
	progressRing->valueNorm = normalizedParamValue();
	fb->addChild(progressRing);

	setBackSvg(backSvg);
}

void Eclipse2Knob::step() {
	app::SvgKnob::step();
	const float bloomAmount = settings::haloBrightness;
	if (std::fabs(bloomAmount - lastBloomAmount) > 1e-4f) {
		lastBloomAmount = bloomAmount;
		if (fb) {
			fb->setDirty();
		}
	}
}

void Eclipse2Knob::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (shadowLayer) {
		shadowLayer->valueNorm = valueNorm;
	}
	if (backLayer) {
		backLayer->valueNorm = valueNorm;
	}
	if (progressRing) {
		progressRing->valueNorm = valueNorm;
	}
	if (fb) {
		fb->setDirty();
	}
}

void Eclipse2Knob::setBackSvg(std::shared_ptr<window::Svg> svg) {
	if (!svg || !fb) return;
	if (!backLayer) {
		backLayer = new EclipseKnob::SvgLayer();
		backLayer->minAngle = minAngle;
		backLayer->maxAngle = maxAngle;
		backLayer->valueNorm = normalizedParamValue();
		backLayer->rotateWithValue = true; // Background/bezel/pointer rotates together
		backLayer->scaleFactor = 0.70f; // Scale down background to prevent clipping in 34x34 box
		fb->addChild(backLayer);
	}
	backLayer->setSvg(svg);
	backLayer->box.size = box.size;
}

float Eclipse2Knob::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

void Eclipse2Knob::setProgressRingBipolar(bool bipolar, float centerNorm) {
	if (!progressRing) return;
	progressRing->bipolar = bipolar;
	progressRing->centerNorm = clamp(centerNorm, 0.f, 1.f);
	if (fb) {
		fb->setDirty();
	}
}

void LeviathanHaloKnob::GlowArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float radiusPx = diameterPx * (17.00f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	if (activeAngle <= startAngle + 0.006f) return;

	nvgSave(args.vg);

	auto drawGlowArc = [&](float glowRadiusPx, float width, NVGpaint paint) {
		const float sweep = activeAngle - startAngle;
		const float delta = width / (2.f * glowRadiusPx);
		nvgBeginPath(args.vg);
		if (sweep <= 2.f * delta) {
			if (sweep > 0.001f) {
				const float midAngle = startAngle + 0.5f * sweep;
				nvgArc(args.vg, center.x, center.y, glowRadiusPx, midAngle - 0.001f, midAngle + 0.001f, NVG_CW);
				nvgStrokePaint(args.vg, paint);
				nvgStrokeWidth(args.vg, width * (sweep / (2.f * delta)));
				nvgStroke(args.vg);
			}
			return;
		}
		nvgArc(args.vg, center.x, center.y, glowRadiusPx, startAngle + delta, activeAngle - delta, NVG_CW);
		nvgStrokePaint(args.vg, paint);
		nvgStrokeWidth(args.vg, width);
		nvgStroke(args.vg);
	};

	nvgLineCap(args.vg, NVG_ROUND);

	NVGpaint glowPaintWide = nvgLinearGradient(args.vg,
		center.x - radiusPx, center.y,
		center.x + radiusPx, center.y,
		nvgRGBA(255, 90, 5, 24),
		nvgRGBA(255, 200, 60, 20));

	NVGpaint glowPaintMedium = nvgLinearGradient(args.vg,
		center.x - radiusPx, center.y,
		center.x + radiusPx, center.y,
		nvgRGBA(255, 110, 10, 56),
		nvgRGBA(255, 215, 80, 42));

	NVGpaint glowPaintTight = nvgLinearGradient(args.vg,
		center.x - radiusPx, center.y,
		center.x + radiusPx, center.y,
		nvgRGBA(255, 130, 15, 110),
		nvgRGBA(255, 230, 100, 85));

	// Draw glow passes at radiusPx so they cast outwards uniformly, masked by the knob body on the inside.
	// Widen tight/medium passes to reach the channel between the inner (18.95px) and outer (21.05px) purple semi-arcs.
	drawGlowArc(radiusPx, std::max(7.5f, diameterPx * (8.0f / 46.f)), glowPaintWide);
	drawGlowArc(radiusPx, std::max(6.5f, diameterPx * (7.2f / 46.f)), glowPaintMedium);
	drawGlowArc(radiusPx, std::max(5.0f, diameterPx * (5.6f / 46.f)), glowPaintTight);

	nvgRestore(args.vg);
}

void LeviathanHaloKnob::LightArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float radiusPx = diameterPx * (17.00f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	if (activeAngle <= startAngle + 0.006f) return;

	nvgSave(args.vg);

	auto beginArcBand = [&](float bandRadiusPx, float bandWidthPx) {
		const float halfWidthPx = 0.5f * bandWidthPx;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, bandRadiusPx + halfWidthPx, startAngle, activeAngle, NVG_CW);
		nvgArc(args.vg, center.x, center.y, bandRadiusPx - halfWidthPx, activeAngle, startAngle, NVG_CCW);
		nvgClosePath(args.vg);
	};

	// 1. Main solid gradient band (flat ends representing the physical material, brighter and fully opaque)
	beginArcBand(radiusPx, std::max(1.5f, diameterPx * (2.25f / 46.f)));
	NVGpaint arcPaint = nvgLinearGradient(args.vg,
		center.x - radiusPx,
		center.y,
		center.x + radiusPx,
		center.y,
		nvgRGBA(255, 175, 45, 255),
		nvgRGBA(255, 242, 160, 255));
	nvgFillPaint(args.vg, arcPaint);
	nvgFill(args.vg);

	// 2. Thin bright highlight inner band (flat ends, pure white hot core highlight)
	beginArcBand(radiusPx - diameterPx * (0.46f / 46.f), std::max(0.35f, diameterPx * (0.38f / 46.f)));
	nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 180));
	nvgFill(args.vg);

	nvgRestore(args.vg);
}

void LeviathanHaloKnob2::GlowArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float mainRadius = diameterPx * (18.15f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.8f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);
	if (bloom <= 0.001f) return;

	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	nvgSave(args.vg);

	auto drawGlowStroke = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto blendColor = [](NVGcolor a, NVGcolor b, float t) {
		t = clamp(t, 0.f, 1.f);
		NVGcolor out;
		out.r = crossfade(a.r, b.r, t);
		out.g = crossfade(a.g, b.g, t);
		out.b = crossfade(a.b, b.b, t);
		out.a = crossfade(a.a, b.a, t);
		return out;
	};

	auto drawSegmentedGlow = [&](float widthPx, NVGcolor cyan, NVGcolor purple) {
		const int segmentCount = 16;
		const float total = endAngle - startAngle;
		const float step = total / float(segmentCount);
		for (int i = 0; i < segmentCount; ++i) {
			const float s0 = startAngle + step * float(i);
			const float s1 = startAngle + step * float(i + 1);
			NVGcolor color = purple;
			if (activeAngle >= s1) {
				color = cyan;
			}
			else if (activeAngle > s0) {
				const float segmentProgress = (activeAngle - s0) / std::max(1e-6f, s1 - s0);
				color = blendColor(purple, cyan, segmentProgress);
			}
			drawGlowStroke(s0, s1, mainRadius, widthPx, color);
		}
	};

	if (foreground) {
		drawSegmentedGlow(std::max(2.2f, 2.7f * scale), bloomColor(config.foregroundOuterActiveColor), bloomColor(config.foregroundOuterInactiveColor));
		drawSegmentedGlow(std::max(1.2f, 1.6f * scale), bloomColor(config.foregroundInnerActiveColor), bloomColor(config.foregroundInnerInactiveColor));
	}
	else {
		drawSegmentedGlow(std::max(5.8f, 6.4f * scale), bloomColor(config.backgroundOuterActiveColor), bloomColor(config.backgroundOuterInactiveColor));
		drawSegmentedGlow(std::max(3.8f, 4.6f * scale), bloomColor(config.backgroundMidActiveColor), bloomColor(config.backgroundMidInactiveColor));
		drawSegmentedGlow(std::max(2.4f, 3.0f * scale), bloomColor(config.backgroundInnerActiveColor), bloomColor(config.backgroundInnerInactiveColor));
	}

	nvgRestore(args.vg);
}

void LeviathanHaloKnob2::LightArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float mainRadius = diameterPx * (18.15f / 46.f);
	const float mainWidth = std::max(1.35f, diameterPx * (1.85f / 46.f));
	const float segmentWidth = mainWidth + 0.95f * scale;
	const float segmentRadius = mainRadius - 0.5f * (segmentWidth - mainWidth);
	const float guideRadius = diameterPx * (20.70f / 46.f);
	const float guideWidth = std::max(0.28f, diameterPx * (0.42f / 46.f));
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.8f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);

	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	nvgSave(args.vg);

	auto drawArcBand = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		const float halfWidthPx = 0.5f * widthPx;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx + halfWidthPx, a0, a1, NVG_CW);
		nvgArc(args.vg, center.x, center.y, radiusPx - halfWidthPx, a1, a0, NVG_CCW);
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
	};

	auto drawGuideArc = [&](float radiusPx, float widthPx, NVGcolor color) {
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, startAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto drawPartialGuideArc = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto drawSegmentBand = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor fill, NVGcolor innerHighlight) {
		if (a1 <= a0) return;
		const float halfWidthPx = 0.5f * widthPx;

		drawArcBand(a0, a1, radiusPx, widthPx + 0.48f * scale, nvgRGBA(0, 0, 4, 218));

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx + halfWidthPx, a0, a1, NVG_CW);
		nvgArc(args.vg, center.x, center.y, radiusPx - halfWidthPx, a1, a0, NVG_CCW);
		nvgClosePath(args.vg);

		NVGpaint segmentPaint = nvgLinearGradient(args.vg,
			center.x + std::cos(a0) * radiusPx,
			center.y + std::sin(a0) * radiusPx,
			center.x + std::cos(a1) * radiusPx,
			center.y + std::sin(a1) * radiusPx,
			fill,
			innerHighlight);
		nvgFillPaint(args.vg, segmentPaint);
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx + halfWidthPx * 0.84f, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(0, 1, 7, 172));
		nvgStrokeWidth(args.vg, std::max(0.13f, widthPx * 0.09f));
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx - halfWidthPx * 0.52f, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, innerHighlight);
		nvgStrokeWidth(args.vg, std::max(0.16f, widthPx * 0.12f));
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto drawSegmentedValueArc = [&]() {
		const int segmentCount = 16;
		const float aStart = startAngle;
		const float aEnd = endAngle;
		const float total = aEnd - aStart;
		const float gap = std::max(0.010f, total * 0.009f);
		const float step = total / float(segmentCount);
		const NVGcolor litCore = config.activeColor;
		const NVGcolor litHot = config.activeHighlightColor;
		const NVGcolor unlitCore = config.inactiveColor;
		const NVGcolor unlitHot = config.inactiveHighlightColor;
		auto blendColor = [](NVGcolor a, NVGcolor b, float t) {
			t = clamp(t, 0.f, 1.f);
			NVGcolor out;
			out.r = crossfade(a.r, b.r, t);
			out.g = crossfade(a.g, b.g, t);
			out.b = crossfade(a.b, b.b, t);
			out.a = crossfade(a.a, b.a, t);
			return out;
		};
		for (int i = 0; i < segmentCount; ++i) {
			const float s0 = aStart + step * float(i) + 0.5f * gap;
			const float s1 = aStart + step * float(i + 1) - 0.5f * gap;
			if (activeAngle <= s0) {
				drawSegmentBand(s0, s1, segmentRadius, segmentWidth, unlitCore, unlitHot);
			}
			else if (activeAngle >= s1) {
				drawSegmentBand(s0, s1, segmentRadius, segmentWidth, litCore, litHot);
			}
			else {
				const float segmentProgress = (activeAngle - s0) / std::max(1e-6f, s1 - s0);
				drawSegmentBand(
					s0,
					s1,
					segmentRadius,
					segmentWidth,
					blendColor(unlitCore, litCore, segmentProgress),
					blendColor(unlitHot, litHot, segmentProgress));
			}
		}
	};

	auto drawSegmentedReflection = [&](float radiusPx, float widthPx, NVGcolor cyan, NVGcolor purple) {
		const int segmentCount = 16;
		const float total = endAngle - startAngle;
		const float step = total / float(segmentCount);
		auto blendColor = [](NVGcolor a, NVGcolor b, float t) {
			t = clamp(t, 0.f, 1.f);
			NVGcolor out;
			out.r = crossfade(a.r, b.r, t);
			out.g = crossfade(a.g, b.g, t);
			out.b = crossfade(a.b, b.b, t);
			out.a = crossfade(a.a, b.a, t);
			return out;
		};
		for (int i = 0; i < segmentCount; ++i) {
			const float s0 = startAngle + step * float(i);
			const float s1 = startAngle + step * float(i + 1);
			NVGcolor color = purple;
			if (activeAngle >= s1) {
				color = cyan;
			}
			else if (activeAngle > s0) {
				const float segmentProgress = (activeAngle - s0) / std::max(1e-6f, s1 - s0);
				color = blendColor(purple, cyan, segmentProgress);
			}
			drawPartialGuideArc(s0, s1, radiusPx, widthPx, color);
		}
	};

	auto drawTerminator = [&](float angle, float direction) {
		const float terminatorSweep = 0.055f;
		const float a0 = angle + std::min(0.f, direction) * terminatorSweep;
		const float a1 = angle + std::max(0.f, direction) * terminatorSweep;
		drawArcBand(a0, a1, segmentRadius, segmentWidth + 1.15f * scale, nvgRGBA(0, 1, 8, 230));
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, segmentRadius - segmentWidth * 0.30f, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(155, 170, 190, 48));
		nvgStrokeWidth(args.vg, std::max(0.16f, 0.22f * scale));
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	const float dipRadius = mainRadius - mainWidth * 1.03f - 0.46f * scale;
	drawArcBand(startAngle, endAngle, mainRadius, mainWidth + 0.92f * scale, nvgRGBA(0, 0, 4, 248));
	drawArcBand(startAngle, endAngle, dipRadius, std::max(0.55f, 0.82f * scale), nvgRGBA(0, 1, 8, 216));
	drawGuideArc(guideRadius, guideWidth, bloomConfig.guideOuterColor);
	drawGuideArc(guideRadius - 0.20f * scale, std::max(0.18f, 0.24f * scale), bloomConfig.guideMidColor);
	drawGuideArc(mainRadius - mainWidth * 0.78f, std::max(0.16f, 0.20f * scale), bloomConfig.guideInnerColor);
	if (bloom > 0.001f) {
		drawSegmentedReflection(dipRadius - 0.18f * scale, std::max(0.28f, 0.38f * scale), bloomColor(bloomConfig.reflectionOuterActiveColor), bloomColor(bloomConfig.reflectionOuterInactiveColor));
		drawSegmentedReflection(dipRadius - 0.52f * scale, std::max(0.12f, 0.17f * scale), bloomColor(bloomConfig.reflectionInnerActiveColor), bloomColor(bloomConfig.reflectionInnerInactiveColor));
	}
	drawGuideArc(dipRadius + 0.46f * scale, std::max(0.15f, 0.22f * scale), nvgRGBA(0, 0, 4, 172));

	drawSegmentedValueArc();
	drawTerminator(startAngle, 1.f);
	drawTerminator(endAngle, -1.f);

	NVGpaint capShadow = nvgRadialGradient(
		args.vg,
		center.x,
		center.y + diameterPx * 0.045f,
		diameterPx * (11.0f / 46.f),
		diameterPx * (16.2f / 46.f),
		nvgRGBA(0, 0, 0, 0),
		nvgRGBA(0, 0, 0, 76));
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, diameterPx * (16.8f / 46.f));
	nvgFillPaint(args.vg, capShadow);
	nvgFill(args.vg);

	nvgRestore(args.vg);
}

void LeviathanHaloKnob2::CapReflectionWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float rimRadius = diameterPx * (14.62f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.8f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);
	if (bloom <= 0.001f) return;

	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	auto strokeArc = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto blendColor = [](NVGcolor a, NVGcolor b, float t) {
		t = clamp(t, 0.f, 1.f);
		NVGcolor out;
		out.r = crossfade(a.r, b.r, t);
		out.g = crossfade(a.g, b.g, t);
		out.b = crossfade(a.b, b.b, t);
		out.a = crossfade(a.a, b.a, t);
		return out;
	};

	auto strokeSegmentedReflection = [&](float radiusPx, float widthPx, NVGcolor cyan, NVGcolor purple) {
		const int segmentCount = 16;
		const float total = endAngle - startAngle;
		const float step = total / float(segmentCount);
		for (int i = 0; i < segmentCount; ++i) {
			const float s0 = startAngle + step * float(i);
			const float s1 = startAngle + step * float(i + 1);
			NVGcolor color = purple;
			if (activeAngle >= s1) {
				color = cyan;
			}
			else if (activeAngle > s0) {
				const float segmentProgress = (activeAngle - s0) / std::max(1e-6f, s1 - s0);
				color = blendColor(purple, cyan, segmentProgress);
			}
			strokeArc(s0, s1, radiusPx, widthPx, color);
		}
	};

	nvgSave(args.vg);

	strokeSegmentedReflection(rimRadius, std::max(0.30f, 0.42f * scale), bloomColor(config.capReflectionOuterActiveColor), bloomColor(config.capReflectionOuterInactiveColor));
	strokeSegmentedReflection(rimRadius - 0.34f * scale, std::max(0.12f, 0.17f * scale), bloomColor(config.capReflectionInnerActiveColor), bloomColor(config.capReflectionInnerInactiveColor));

	nvgRestore(args.vg);
}

void LeviathanHaloKnob::RimHighlightWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float rimRadiusPx = diameterPx * (14.18f / 46.f);
	const float innerRimRadiusPx = diameterPx * (13.54f / 46.f);

	auto strokeArc = [&](float radiusPx, float startRad, float endRad, float widthPx, NVGcolor color) {
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, startRad, endRad, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgStroke(args.vg);
	};
	auto withAlpha = [](int r, int g, int b, float alpha) {
		return nvgRGBA(r, g, b, clamp((int)std::round(alpha), 0, 255));
	};
	auto triangle = [&](float phaseOffset) {
		const float p = levi_math::wrap01(valueNorm + phaseOffset);
		return 1.f - std::fabs(2.f * p - 1.f);
	};

	const float violetReveal = triangle(0.10f);
	const float blueReveal = triangle(0.43f);
	const float lavenderReveal = triangle(0.76f);

	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);
	nvgRotate(args.vg, 0.03f * crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f)));
	nvgTranslate(args.vg, -center.x, -center.y);
	nvgLineCap(args.vg, NVG_ROUND);

	strokeArc(rimRadiusPx, -0.74f * M_PI, 0.25f * M_PI, std::max(0.48f, 0.74f * scale), withAlpha(62, 44, 126, 76.f + 42.f * violetReveal));
	strokeArc(rimRadiusPx, -0.66f * M_PI, 0.18f * M_PI, std::max(0.30f, 0.42f * scale), withAlpha(118, 84, 196, 62.f + 54.f * violetReveal));
	strokeArc(innerRimRadiusPx, -0.68f * M_PI, 0.13f * M_PI, std::max(0.20f, 0.30f * scale), withAlpha(82, 68, 166, 52.f + 42.f * blueReveal));
	strokeArc(rimRadiusPx, -0.54f * M_PI, -0.03f * M_PI, std::max(0.16f, 0.22f * scale), withAlpha(178, 148, 232, 54.f + 60.f * lavenderReveal));
	strokeArc(rimRadiusPx + 0.34f * scale, -0.15f * M_PI, 0.05f * M_PI, std::max(0.14f, 0.20f * scale), withAlpha(218, 198, 252, 64.f + 58.f * lavenderReveal));

	nvgRestore(args.vg);

	nvgSave(args.vg);
	nvgLineCap(args.vg, NVG_ROUND);
	strokeArc(rimRadiusPx, -0.58f * M_PI, 0.20f * M_PI, std::max(0.16f, 0.22f * scale), nvgRGBA(178, 142, 232, 44));
	strokeArc(innerRimRadiusPx, -0.62f * M_PI, 0.10f * M_PI, std::max(0.14f, 0.18f * scale), nvgRGBA(96, 82, 174, 34));
	nvgRestore(args.vg);
}

LeviathanHaloKnob::LeviathanHaloKnob() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobBack.svg");
	app::SvgKnob::setSvg(backSvg);
	box.size = Vec(46.f, 46.f);
	if (fb) {
		fb->box.size = box.size;
	}
	if (sw) {
		sw->hide();
	}
	if (shadow) {
		shadow->opacity = 0.f;
	}

	shadowLayer = new EclipseKnob::ShadowWidget();
	shadowLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/LeviathanHaloKnobShadow.svg"));
	shadowLayer->box.size = box.size;
	shadowLayer->minAngle = minAngle;
	shadowLayer->maxAngle = maxAngle;
	shadowLayer->valueNorm = normalizedParamValue();
	fb->addChild(shadowLayer);

	backLayer = new EclipseKnob::SvgLayer();
	backLayer->setSvg(backSvg);
	backLayer->box.size = box.size;
	backLayer->minAngle = minAngle;
	backLayer->maxAngle = maxAngle;
	backLayer->valueNorm = normalizedParamValue();
	backLayer->rotateWithValue = false;
	fb->addChild(backLayer);

	glowArc = new GlowArcWidget();
	glowArc->box.size = box.size;
	glowArc->minAngle = minAngle;
	glowArc->maxAngle = maxAngle;
	glowArc->valueNorm = normalizedParamValue();
	fb->addChild(glowArc);

	lightArc = new LightArcWidget();
	lightArc->box.size = box.size;
	lightArc->minAngle = minAngle;
	lightArc->maxAngle = maxAngle;
	lightArc->valueNorm = normalizedParamValue();
	fb->addChild(lightArc);

	centerLayer = new EclipseKnob::SvgLayer();
	centerLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenter.svg"));
	centerLayer->box.size = box.size;
	centerLayer->minAngle = minAngle;
	centerLayer->maxAngle = maxAngle;
	centerLayer->valueNorm = normalizedParamValue();
	centerLayer->rotateWithValue = true;
	fb->addChild(centerLayer);

	rimHighlight = new RimHighlightWidget();
	rimHighlight->box.size = box.size;
	rimHighlight->minAngle = minAngle;
	rimHighlight->maxAngle = maxAngle;
	rimHighlight->valueNorm = normalizedParamValue();
	fb->addChild(rimHighlight);
}

void LeviathanHaloKnob::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (backLayer) {
		backLayer->valueNorm = valueNorm;
	}
	if (centerLayer) {
		centerLayer->valueNorm = valueNorm;
	}
	if (glowArc) {
		glowArc->valueNorm = valueNorm;
	}
	if (lightArc) {
		lightArc->valueNorm = valueNorm;
	}
	if (rimHighlight) {
		rimHighlight->valueNorm = valueNorm;
	}
	if (fb) {
		fb->setDirty();
	}
}

float LeviathanHaloKnob::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

LeviathanHaloKnob2::LeviathanHaloKnob2() : LeviathanHaloKnob2(Config()) {
}

LeviathanHaloKnob2::Config LeviathanHaloKnob2::brightOrangeConfig() {
	Config config;
	config.ledArc.activeColor = nvgRGBA(255, 184, 0, 255);
	config.ledArc.activeHighlightColor = nvgRGBA(255, 232, 82, 232);
	config.ledArc.inactiveColor = nvgRGBA(158, 58, 16, 216);
	config.ledArc.inactiveHighlightColor = nvgRGBA(220, 94, 30, 168);
	config.bloom.backgroundOuterActiveColor = nvgRGBA(255, 148, 0, 50);
	config.bloom.backgroundOuterInactiveColor = nvgRGBA(130, 42, 10, 30);
	config.bloom.backgroundMidActiveColor = nvgRGBA(255, 172, 0, 80);
	config.bloom.backgroundMidInactiveColor = nvgRGBA(160, 50, 12, 50);
	config.bloom.backgroundInnerActiveColor = nvgRGBA(255, 204, 20, 122);
	config.bloom.backgroundInnerInactiveColor = nvgRGBA(204, 68, 16, 72);
	config.bloom.foregroundOuterActiveColor = nvgRGBA(255, 214, 34, 74);
	config.bloom.foregroundOuterInactiveColor = nvgRGBA(206, 72, 18, 44);
	config.bloom.foregroundInnerActiveColor = nvgRGBA(255, 244, 118, 62);
	config.bloom.foregroundInnerInactiveColor = nvgRGBA(236, 104, 34, 32);
	config.bloom.reflectionOuterActiveColor = nvgRGBA(255, 174, 0, 70);
	config.bloom.reflectionOuterInactiveColor = nvgRGBA(144, 44, 10, 68);
	config.bloom.reflectionInnerActiveColor = nvgRGBA(255, 224, 36, 62);
	config.bloom.reflectionInnerInactiveColor = nvgRGBA(218, 86, 22, 48);
	config.bloom.guideOuterColor = nvgRGBA(255, 210, 38, 84);
	config.bloom.guideMidColor = nvgRGBA(186, 58, 14, 58);
	config.bloom.guideInnerColor = nvgRGBA(255, 238, 98, 68);
	config.bloom.capReflectionOuterActiveColor = nvgRGBA(255, 188, 0, 96);
	config.bloom.capReflectionOuterInactiveColor = nvgRGBA(188, 62, 16, 82);
	config.bloom.capReflectionInnerActiveColor = nvgRGBA(255, 240, 108, 72);
	config.bloom.capReflectionInnerInactiveColor = nvgRGBA(236, 106, 36, 54);
	return config;
}

LeviathanHaloKnob2::LeviathanHaloKnob2(Config config) : config(config) {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnob2Back.svg");
	centerNormalSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenter.svg");
	centerLitSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenterLit.svg");
	app::SvgKnob::setSvg(backSvg);
	box.size = Vec(46.f, 46.f);
	if (fb) {
		fb->box.size = box.size;
	}
	if (sw) {
		sw->hide();
	}
	if (shadow) {
		shadow->opacity = 0.f;
	}
	lastBloomAmount = settings::haloBrightness;

	backLayer = new EclipseKnob::SvgLayer();
	backLayer->setSvg(backSvg);
	backLayer->box.size = box.size;
	backLayer->minAngle = minAngle;
	backLayer->maxAngle = maxAngle;
	backLayer->valueNorm = normalizedParamValue();
	backLayer->rotateWithValue = false;
	fb->addChild(backLayer);

	glowArc = new GlowArcWidget();
	glowArc->box.size = box.size;
	glowArc->minAngle = minAngle;
	glowArc->maxAngle = maxAngle;
	glowArc->valueNorm = normalizedParamValue();
	glowArc->config = this->config.bloom;
	fb->addChild(glowArc);

	lightArc = new LightArcWidget();
	lightArc->box.size = box.size;
	lightArc->minAngle = minAngle;
	lightArc->maxAngle = maxAngle;
	lightArc->valueNorm = normalizedParamValue();
	lightArc->config = this->config.ledArc;
	lightArc->bloomConfig = this->config.bloom;
	fb->addChild(lightArc);

	foregroundGlowArc = new GlowArcWidget();
	foregroundGlowArc->box.size = box.size;
	foregroundGlowArc->minAngle = minAngle;
	foregroundGlowArc->maxAngle = maxAngle;
	foregroundGlowArc->valueNorm = normalizedParamValue();
	foregroundGlowArc->foreground = true;
	foregroundGlowArc->config = this->config.bloom;
	fb->addChild(foregroundGlowArc);

	centerLayer = new EclipseKnob::SvgLayer();
	centerLayer->setSvg(centerNormalSvg);
	centerLayer->box.size = box.size;
	centerLayer->minAngle = minAngle;
	centerLayer->maxAngle = maxAngle;
	centerLayer->valueNorm = normalizedParamValue();
	centerLayer->rotateWithValue = true;
	fb->addChild(centerLayer);

	capReflection = new CapReflectionWidget();
	capReflection->box.size = box.size;
	capReflection->minAngle = minAngle;
	capReflection->maxAngle = maxAngle;
	capReflection->valueNorm = normalizedParamValue();
	capReflection->config = this->config.bloom;
	fb->addChild(capReflection);

}

void LeviathanHaloKnob2::updateCenterSvg() {
	const bool shouldLight = hovered || dragging;
	if (centerLit == shouldLight) {
		return;
	}
	centerLit = shouldLight;
	if (centerLayer) {
		centerLayer->setSvg(centerLit ? centerLitSvg : centerNormalSvg);
		centerLayer->box.size = box.size;
		centerLayer->valueNorm = normalizedParamValue();
	}
	if (fb) {
		fb->setDirty();
	}
}

void LeviathanHaloKnob2::step() {
	app::SvgKnob::step();
	const float bloomAmount = settings::haloBrightness;
	if (std::fabs(bloomAmount - lastBloomAmount) > 1e-4f) {
		lastBloomAmount = bloomAmount;
		if (fb) {
			fb->setDirty();
		}
	}
}

void LeviathanHaloKnob2::onEnter(const event::Enter& e) {
	hovered = true;
	updateCenterSvg();
	app::SvgKnob::onEnter(e);
}

void LeviathanHaloKnob2::onLeave(const event::Leave& e) {
	hovered = false;
	updateCenterSvg();
	app::SvgKnob::onLeave(e);
}

void LeviathanHaloKnob2::onDragStart(const event::DragStart& e) {
	dragging = true;
	updateCenterSvg();
	app::SvgKnob::onDragStart(e);
}

void LeviathanHaloKnob2::onDragEnd(const event::DragEnd& e) {
	dragging = false;
	updateCenterSvg();
	app::SvgKnob::onDragEnd(e);
}

void LeviathanHaloKnob2::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (backLayer) {
		backLayer->valueNorm = valueNorm;
	}
	if (centerLayer) {
		centerLayer->valueNorm = valueNorm;
	}
	if (capReflection) {
		capReflection->valueNorm = valueNorm;
	}
	if (glowArc) {
		glowArc->valueNorm = valueNorm;
	}
	if (foregroundGlowArc) {
		foregroundGlowArc->valueNorm = valueNorm;
	}
	if (lightArc) {
		lightArc->valueNorm = valueNorm;
	}
	if (fb) {
		fb->setDirty();
	}
}

float LeviathanHaloKnob2::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

ClockworkGearKnob::CogwheelWidget::CogwheelWidget() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
}

void ClockworkGearKnob::CogwheelWidget::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void ClockworkGearKnob::CogwheelWidget::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 0.f) return;

	const float scale = diameterPx / std::max(svgSize.x, svgSize.y);
	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);
	nvgRotate(args.vg, angleRad);
	nvgScale(args.vg, scale, scale);
	nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
	Widget::draw(args);
	nvgRestore(args.vg);
}

ClockworkGearKnob::ClockworkGearKnob() {
	primaryCogwheel = new CogwheelWidget();
	secondaryCogwheel = new CogwheelWidget();
	primaryCogwheel->box.size = box.size;
	secondaryCogwheel->box.size = box.size;
	try {
		primaryCogwheel->setSvg(visual_assets::loadPluginSvgCached("res/icon/cogwheel_amythyst.svg"));
	}
	catch (const std::exception& e) {
		WARN("Failed to load cogwheel-backed gear knob SVG: %s", e.what());
		primaryCogwheel->setSvg(nullptr);
	}
	try {
		secondaryCogwheel->setSvg(visual_assets::loadPluginSvgCached("res/icon/cogwheel_grandidierite.svg"));
	}
	catch (const std::exception& e) {
		WARN("Failed to load secondary cogwheel-backed gear knob SVG: %s", e.what());
		secondaryCogwheel->setSvg(nullptr);
	}
	updateCogwheelGeometry();
	fb->addChildBelow(primaryCogwheel, tw);
	fb->addChildBelow(secondaryCogwheel, tw);
}

void ClockworkGearKnob::draw(const DrawArgs& args) {
	GearKnobInvertSized::draw(args);
}

void ClockworkGearKnob::onChange(const ChangeEvent& e) {
	GearKnobInvertSized::onChange(e);
	updateCogwheelGeometry();
	if (fb) {
		fb->setDirty();
	}
}

void ClockworkGearKnob::updateCogwheelGeometry() {
	if (!primaryCogwheel || !secondaryCogwheel) return;
	const float primaryDiameterPx = 17.f;
	const float secondaryDiameterPx = primaryDiameterPx * 0.5f;
	const float valueNorm = normalizedParamValue();
	const float knobAngle = crossfade(minAngle, maxAngle, valueNorm);
	const Vec cogwheelOffset(0.f, -1.25f);
	const Vec primaryPos = box.size.mult(0.5f).plus(cogwheelOffset);
	const Vec primaryCenter = primaryPos.plus(Vec(0.5f * primaryDiameterPx, 0.5f * primaryDiameterPx));

	primaryCogwheel->center = primaryCenter;
	primaryCogwheel->diameterPx = primaryDiameterPx;
	primaryCogwheel->angleRad = -knobAngle;

	const float centerDistancePx = 0.5f * (primaryDiameterPx + secondaryDiameterPx) - 0.45f;
	const float secondaryCenterPhaseOffsetRad = -0.10f;
	const float secondaryCenterCos = std::cos(secondaryCenterPhaseOffsetRad);
	const float secondaryCenterSin = std::sin(secondaryCenterPhaseOffsetRad);
	const Vec secondaryDirection(-0.9636305f, 0.2672384f);
	const Vec secondaryCenter = primaryCenter.plus(Vec(
		(secondaryDirection.x * secondaryCenterCos - secondaryDirection.y * secondaryCenterSin) * centerDistancePx,
		(secondaryDirection.x * secondaryCenterSin + secondaryDirection.y * secondaryCenterCos) * centerDistancePx));
	const float secondaryGearRatio = primaryDiameterPx / secondaryDiameterPx;
	const float secondaryToothPhaseOffsetRad = secondaryCenterPhaseOffsetRad * (1.f + secondaryGearRatio);
	secondaryCogwheel->center = secondaryCenter;
	secondaryCogwheel->diameterPx = secondaryDiameterPx;
	secondaryCogwheel->angleRad = knobAngle * secondaryGearRatio + secondaryToothPhaseOffsetRad;
}
