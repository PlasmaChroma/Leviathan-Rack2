#define NANOVG_GL2
#include "Eclipse2RuntimeBake.hpp"
#include "../render/AdaptiveGlSurface.hpp"
#include <nanovg_gl.h>
#include <GLFW/glfw3.h>

namespace eclipse2_bake {
bool Raster::prepare(Draw draw, void* user) {
	return prepareSized(draw, user, Vec(34,34), 4.f);
}
bool Raster::prepareSized(Draw draw, void* user, Vec logicalSize, float density) {
	if (!rgba.empty()) return true;
	if (attempted || !draw || !APP || !APP->scene || !APP->window
		|| !APP->window->vg || !APP->window->win
		|| glfwGetCurrentContext() != APP->window->win) return false;
	attempted = true;
	struct Bake {
		Draw draw;
		void* user;
		std::vector<unsigned char> bottomUp;
		bool complete;
		Vec logicalSize;
		float density;
		int width, height;
	} bake{draw, user, std::vector<unsigned char>(size_t(logicalSize.x*density) * size_t(logicalSize.y*density) * 4), false,
		logicalSize, density, int(logicalSize.x*density), int(logicalSize.y*density)};
	visual_assets::AdaptiveGlSurface target;
	visual_assets::AdaptiveGlSurfacePolicy policy;
	policy.minDensity = policy.maxDensity = density;
	policy.sizeQuantum = 1;
	policy.vertexAttributeCount = 2; // NanoVG position and UV attributes.
	// The shared GL guard preserves the host FBO and recorder's driver state.
	// A private NanoVG recorder permits first use even inside a dirty outer FBO.
	const bool rendered = target.renderIfNeeded(APP->window->vg, logicalSize, 1.f, 1.f,
		policy, true, [](void* data, Vec, int viewportY) {
			auto& bake = *static_cast<Bake*>(data);
			NVGcontext* vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
			if (!vg) return;
			glViewport(0, viewportY, bake.width, bake.height);
			glDisable(GL_SCISSOR_TEST);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glStencilMask(~0u);
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			nvgBeginFrame(vg,bake.logicalSize.x,bake.logicalSize.y,bake.density);
			Widget::DrawArgs args;
			args.vg = vg;
			args.clipBox = Rect::inf();
			bake.draw(args,bake.user);
			nvgEndFrame(vg);
			GLint packBuffer = 0;
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING,&packBuffer);
			glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
			// Remaining pack state is saved/restored by AdaptiveGlSurface's guard.
			glPixelStorei(GL_PACK_ALIGNMENT,1);
			glPixelStorei(GL_PACK_ROW_LENGTH,0);
			glPixelStorei(GL_PACK_SKIP_ROWS,0);
			glPixelStorei(GL_PACK_SKIP_PIXELS,0);
			glReadPixels(0,viewportY,bake.width,bake.height,GL_RGBA,GL_UNSIGNED_BYTE,bake.bottomUp.data());
			glBindBuffer(GL_PIXEL_PACK_BUFFER,GLuint(packBuffer));
			nvgDeleteGL2(vg);
			bake.complete = true;
		}, &bake);
	target.reset(true);
	if (!rendered || !bake.complete) return false;
	rgba.resize(bake.bottomUp.size());
	for (int y=0;y<bake.height;++y)
		std::copy(bake.bottomUp.data()+size_t(bake.height-1-y)*bake.width*4,
			bake.bottomUp.data()+size_t(bake.height-y)*bake.width*4,rgba.data()+size_t(y)*bake.width*4);
	++builds;
	return true;
}
}
