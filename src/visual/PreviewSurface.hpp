#pragma once

#include "../plugin.hpp"

// Shared interior treatment for preview widgets whose outer frame is supplied
// by createPreviewFrameEnhancementWidget(). Keeping the fill and grid here
// prevents the panel-surface layer from being drawn and then hidden below an
// opaque preview framebuffer.
namespace preview_surface {

inline void drawNvgGrid(NVGcontext* vg, Vec size) {
	const float w = size.x;
	const float h = size.y;
	if (w <= 1.f || h <= 1.f) return;
	const int majorCols = std::max(3, int(std::round(w / 16.f)));
	const int majorRows = std::max(3, int(std::round(h / 16.f)));
	const float majorX = w / float(majorCols);
	const float majorY = h / float(majorRows);
	nvgBeginPath(vg);
	for (int col = 0; col < majorCols; ++col) for (int sub = 1; sub < 4; ++sub) {
		const float x = float(col) * majorX + majorX * (float(sub) * 0.25f);
		nvgMoveTo(vg, x, 0.f); nvgLineTo(vg, x, h);
	}
	for (int row = 0; row < majorRows; ++row) for (int sub = 1; sub < 4; ++sub) {
		const float y = float(row) * majorY + majorY * (float(sub) * 0.25f);
		nvgMoveTo(vg, 0.f, y); nvgLineTo(vg, w, y);
	}
	nvgStrokeWidth(vg, 0.38f);
	nvgStrokeColor(vg, nvgRGBA(0x1c, 0xcc, 0xd9, 30));
	nvgStroke(vg);
	nvgBeginPath(vg);
	for (int col = 1; col < majorCols; ++col) {
		const float x = float(col) * majorX;
		nvgMoveTo(vg, x, 0.f); nvgLineTo(vg, x, h);
	}
	for (int row = 1; row < majorRows; ++row) {
		const float y = float(row) * majorY;
		nvgMoveTo(vg, 0.f, y); nvgLineTo(vg, w, y);
	}
	nvgStrokeWidth(vg, 0.55f);
	nvgStrokeColor(vg, nvgRGBA(0x72, 0x8d, 0xff, 46));
	nvgStroke(vg);
}

inline void drawNvgOpaqueGrid(NVGcontext* vg, Vec size) {
	nvgBeginPath(vg);
	nvgRect(vg, 0.f, 0.f, size.x, size.y);
	nvgFillColor(vg, nvgRGB(0, 0, 0));
	nvgFill(vg);
	drawNvgGrid(vg, size);
}

struct CachedOpaqueGridWidget final : TransparentWidget {
	void draw(const DrawArgs& args) override {
		drawNvgOpaqueGrid(args.vg, box.size);
	}
};

struct CachedOpaqueGridFramebuffer final : widget::FramebufferWidget {
	void draw(const DrawArgs& args) override {
		// Framebuffer textures remain alpha-bearing even when their cached child
		// fills every pixel. Establish a direct opaque barrier first so animated
		// panel layers can never leak through transparent edge texels or a cache
		// compositing path that preserves alpha.
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGB(0, 0, 0));
		nvgFill(args.vg);
		widget::FramebufferWidget::draw(args);
	}
};

inline widget::FramebufferWidget* createCachedOpaqueGrid(Vec size) {
	auto* framebuffer = new CachedOpaqueGridFramebuffer();
	framebuffer->box.size = size;
	framebuffer->dirtyOnSubpixelChange = false;
	auto* surface = new CachedOpaqueGridWidget();
	surface->box.size = size;
	framebuffer->addChild(surface);
	return framebuffer;
}

inline void drawGlGrid(Vec size) {
	const float w = size.x;
	const float h = size.y;
	if (w <= 1.f || h <= 1.f) return;
	const int majorCols = std::max(3, int(std::round(w / 16.f)));
	const int majorRows = std::max(3, int(std::round(h / 16.f)));
	const float majorX = w / float(majorCols);
	const float majorY = h / float(majorRows);
	glLineWidth(0.38f);
	glColor4f(0x1c / 255.f, 0xcc / 255.f, 0xd9 / 255.f, 30.f / 255.f);
	glBegin(GL_LINES);
	for (int col = 0; col < majorCols; ++col) for (int sub = 1; sub < 4; ++sub) {
		const float x = float(col) * majorX + majorX * (float(sub) * 0.25f);
		glVertex2f(x, 0.f); glVertex2f(x, h);
	}
	for (int row = 0; row < majorRows; ++row) for (int sub = 1; sub < 4; ++sub) {
		const float y = float(row) * majorY + majorY * (float(sub) * 0.25f);
		glVertex2f(0.f, y); glVertex2f(w, y);
	}
	glEnd();
	glLineWidth(0.55f);
	glColor4f(0x72 / 255.f, 0x8d / 255.f, 1.f, 46.f / 255.f);
	glBegin(GL_LINES);
	for (int col = 1; col < majorCols; ++col) {
		const float x = float(col) * majorX;
		glVertex2f(x, 0.f); glVertex2f(x, h);
	}
	for (int row = 1; row < majorRows; ++row) {
		const float y = float(row) * majorY;
		glVertex2f(0.f, y); glVertex2f(w, y);
	}
	glEnd();
}

} // namespace preview_surface
