#include "AdaptiveGlSurface.hpp"

#include "../GlLifecycleUtils.hpp"
#include "../NvgGraphicsLifecycle.hpp"

#include <nanovg_gl.h>

namespace visual_assets {

namespace {
// Compatibility GL2 state plus the generic vertex inputs used by our callbacks.
struct SurfaceStateGuard {
	GLint previousFramebuffer = 0;
	GLint previousReadFramebuffer = 0;
	GLint previousRenderbuffer = 0;
	GLint previousProgram = 0;
	GLint previousArrayBuffer = 0;
	GLint previousActiveTexture = GL_TEXTURE0;
	GLint previousTexture2d = 0;
	GLint previousMatrixMode = GL_MODELVIEW;

	struct Attribute { GLint enabled, size, type, normalized, stride, buffer; void* pointer; } attributes[4];
	GLint texture0 = 0;
	GLint unpackBuffer = 0;
	int attributeCount = 0;
	SurfaceStateGuard(int count) : attributeCount(clamp(count, 0, 4)) {
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousReadFramebuffer);
		glGetIntegerv(GL_RENDERBUFFER_BINDING, &previousRenderbuffer);
		glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArrayBuffer);
		glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture2d);
		glGetIntegerv(GL_MATRIX_MODE, &previousMatrixMode);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glActiveTexture(GL_TEXTURE0);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture0);
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &unpackBuffer);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		for (GLuint i = 0; i < GLuint(attributeCount); ++i) {
			Attribute& a = attributes[i];
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &a.enabled);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_SIZE, &a.size);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_TYPE, &a.type);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &a.normalized);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &a.stride);
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &a.buffer);
			glGetVertexAttribPointerv(i, GL_VERTEX_ATTRIB_ARRAY_POINTER, &a.pointer);
		}
	}
	~SurfaceStateGuard() {
		for (GLuint i = 0; i < GLuint(attributeCount); ++i) {
			const Attribute& a = attributes[i];
			glBindBuffer(GL_ARRAY_BUFFER, GLuint(a.buffer));
			glVertexAttribPointer(i, a.size, GLenum(a.type), GLboolean(a.normalized), a.stride, a.pointer);
			if (a.enabled) glEnableVertexAttribArray(i); else glDisableVertexAttribArray(i);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GLuint(unpackBuffer));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GLuint(texture0));
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GLenum(previousMatrixMode));
		glPopClientAttrib();
		// Saved draw/read-buffer selections belong to the incoming FBOs.
		// Restoring them while our offscreen FBO is bound is invalid (e.g. GL_BACK).
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLuint(previousFramebuffer));
		glBindFramebuffer(GL_READ_FRAMEBUFFER, GLuint(previousReadFramebuffer));
		glBindRenderbuffer(GL_RENDERBUFFER, GLuint(previousRenderbuffer));
		glPopAttrib();
		glUseProgram(GLuint(previousProgram));
		glBindBuffer(GL_ARRAY_BUFFER, GLuint(previousArrayBuffer));
		glActiveTexture(GLenum(previousActiveTexture));
		glBindTexture(GL_TEXTURE_2D, GLuint(previousTexture2d));

	}
};
}


AdaptiveGlSurface::~AdaptiveGlSurface() {
	// Widget teardown is not guaranteed to run with the owning context current.
	reset(false);
}

void AdaptiveGlSurface::reset(bool deleteGlObjects) {
	if (deleteGlObjects && gl_lifecycle::resourceContextIsCurrent(resourceContext)) {
		if (front) nvgluDeleteFramebuffer(front);
		if (back) nvgluDeleteFramebuffer(back);
	} else {
		gl_lifecycle::retireFramebuffer(resourceContext, front);
		gl_lifecycle::retireFramebuffer(resourceContext, back);
	}
	resourceContext.reset();
	front = nullptr;
	back = nullptr;
	vg = nullptr;
	frontCapacityWidth = 0;
	frontCapacityHeight = 0;
	backCapacityWidth = 0;
	backCapacityHeight = 0;
	frontActiveWidth = 0;
	frontActiveHeight = 0;
	dirty = true;
	surfaceGeneration = 0;
}

bool AdaptiveGlSurface::ensureBackSurface(
	NVGcontext* targetVg, int width, int height, bool validate) {
	if (!targetVg || width < 1 || height < 1) return false;
	if (vg != targetVg || !gl_lifecycle::resourceContextMatches(resourceContext, targetVg)) {
		// The old context owns its handles. Forget them without issuing driver
		// calls if Rack replaced the editor without delivering its destroy event.
		reset(false);
		vg = targetVg;
		resourceContext = gl_lifecycle::acquireResourceContext(targetVg);
	}

	bool matches = back && backCapacityWidth == width && backCapacityHeight == height;
	if (matches && validate) {
		matches = gl_lifecycle::isValidTextureFramebufferPair(back->texture, back->fbo)
			&& nvg_gfx_lifecycle::ownedNvgImageSizeMatches(
				targetVg, back->image, width, height);
	}
	if (!matches) {
		if (back) gl_lifecycle::retireFramebuffer(resourceContext, back);
		back = nvgluCreateFramebuffer(targetVg, width, height, 0);
		backCapacityWidth = back ? width : 0;
		backCapacityHeight = back ? height : 0;
	}
	return back != nullptr;
}

bool AdaptiveGlSurface::renderIfNeeded(NVGcontext* targetVg,
	Vec logicalSize,
	float rackZoom,
	float windowPixelRatio,
	const AdaptiveGlSurfacePolicy& policy,
	bool validate,
	RenderCallback callback,
	void* user) {
	if (!targetVg || !callback || logicalSize.x <= 0.f || logicalSize.y <= 0.f) return false;
	if (vg != targetVg || !gl_lifecycle::resourceContextMatches(resourceContext, targetVg)) {
		reset(false);
		vg = targetVg;
		resourceContext = gl_lifecycle::acquireResourceContext(targetVg);
	}

	const float maxDensity = std::max(0.01f, policy.maxDensity);
	const float minDensity = clamp(policy.minDensity, 0.01f, maxDensity);
	const int quantum = std::max(1, policy.sizeQuantum);
	int capacityWidth = std::max(1, int(std::ceil(logicalSize.x * maxDensity)));
	int capacityHeight = std::max(1, int(std::ceil(logicalSize.y * maxDensity)));
	if (policy.retainPeakCapacity) {
		capacityWidth = std::max(capacityWidth,
			std::max(frontCapacityWidth, backCapacityWidth));
		capacityHeight = std::max(capacityHeight,
			std::max(frontCapacityHeight, backCapacityHeight));
	}
	const float pixelRatio = std::max(1.f, std::floor(windowPixelRatio));
	const float density = clamp(std::max(rackZoom, 1e-4f) * pixelRatio, minDensity, maxDensity);
	auto quantizedExtent = [quantum](float logicalExtent, int capacity) {
		const int requested = std::max(1, int(std::ceil(logicalExtent)));
		const int quantized = ((requested + quantum - 1) / quantum) * quantum;
		return std::min(capacity, quantized);
	};
	const int activeWidth = quantizedExtent(logicalSize.x * density, capacityWidth);
	const int activeHeight = quantizedExtent(logicalSize.y * density, capacityHeight);
	const bool resolutionGrowth = activeWidth > frontActiveWidth || activeHeight > frontActiveHeight;
	if (!dirty && !resolutionGrowth) return false;
	SurfaceStateGuard stateGuard(policy.vertexAttributeCount);
	if (!ensureBackSurface(targetVg, capacityWidth, capacityHeight, validate)) return false;

	nvgluBindFramebuffer(back);
	// The active image can occupy only a prefix of a retained larger texture.
	// Clear the whole target so linear filtering at that boundary cannot sample
	// pixels left behind by a previous, larger active render.
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, 0, backCapacityWidth, backCapacityHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	// NVGLU marks framebuffer images FLIPY for NanoVG. Rendering against the
	// top edge makes the active prefix addressable with a larger image pattern.
	callback(user, Vec(float(activeWidth), float(activeHeight)), capacityHeight - activeHeight);


	std::swap(front, back);
	std::swap(frontCapacityWidth, backCapacityWidth);
	std::swap(frontCapacityHeight, backCapacityHeight);
	frontActiveWidth = activeWidth;
	frontActiveHeight = activeHeight;
	dirty = false;
	++surfaceGeneration;
	return true;
}

bool AdaptiveGlSurface::draw(const Widget::DrawArgs& args, Vec logicalSize, float alpha) const {
	if (args.vg != vg || !gl_lifecycle::resourceContextMatches(resourceContext, vg)
		|| !front || front->image <= 0 || frontActiveWidth < 1 || frontActiveHeight < 1
		|| frontCapacityWidth < 1 || frontCapacityHeight < 1) return false;
	const float patternWidth = logicalSize.x * float(frontCapacityWidth) / float(frontActiveWidth);
	const float patternHeight = logicalSize.y * float(frontCapacityHeight) / float(frontActiveHeight);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, logicalSize.x, logicalSize.y);
	nvgFillPaint(args.vg, nvgImagePattern(
		args.vg, 0.f, 0.f, patternWidth, patternHeight, 0.f, front->image, alpha));
	nvgFill(args.vg);
	return true;
}

} // namespace visual_assets
