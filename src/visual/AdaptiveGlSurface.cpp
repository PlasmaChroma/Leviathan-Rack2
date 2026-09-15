#include "AdaptiveGlSurface.hpp"

#include "../GlLifecycleUtils.hpp"
#include "../NvgGraphicsLifecycle.hpp"

#include <nanovg_gl.h>
#include <new>
#include <chrono>

namespace visual_assets {

namespace {
struct PhaseTimer {
    using Clock = std::chrono::steady_clock;
    uint64_t* total;
    Clock::time_point start;
    explicit PhaseTimer(uint64_t* value):total(value),start(value?Clock::now():Clock::time_point()){}
    ~PhaseTimer(){if(total)*total+=uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-start).count());}
};
// Lightweight state guard capturing only what our shader passes mutate,
// avoiding deprecated glPushAttrib and synchronous vertex attribute pointer queries.
struct SurfaceStateGuard {
	GLint previousFramebuffer = 0;
	GLint previousReadFramebuffer = 0;
	GLint previousRenderbuffer = 0;
	GLint previousProgram = 0;
	GLint previousArrayBuffer = 0;
	GLint previousActiveTexture = GL_TEXTURE0;
	GLint previousTexture2d = 0;
	GLint previousMatrixMode = GL_MODELVIEW;

	GLint texture0 = 0;
	GLint unpackBuffer = 0;
	GLint unpackAlignment = 4;
	GLint previousViewport[4] = {0, 0, 0, 0};
	GLint previousScissor[4] = {0, 0, 0, 0};
	GLint previousBlendSrcRgb = GL_ONE, previousBlendDstRgb = GL_ZERO;
	GLint previousBlendSrcAlpha = GL_ONE, previousBlendDstAlpha = GL_ZERO;
	GLint previousBlendEqRgb = GL_FUNC_ADD, previousBlendEqAlpha = GL_FUNC_ADD;
	GLboolean previousScissorEnabled = GL_FALSE;
	GLboolean previousBlendEnabled = GL_FALSE;
	GLboolean previousDepthEnabled = GL_FALSE;
	GLboolean previousStencilEnabled = GL_FALSE;
	GLboolean previousCullEnabled = GL_FALSE;
	GLint attributeEnabled[4] = {0, 0, 0, 0};
	int attributeCount = 0;
	bool shaderOnly = false;

	SurfaceStateGuard(int count, bool restricted = false)
		: attributeCount(count < 0 ? -1 : clamp(count, 0, 4)), shaderOnly(restricted) {
		if (attributeCount < 0) return;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousReadFramebuffer);
		glGetIntegerv(GL_RENDERBUFFER_BINDING, &previousRenderbuffer);
		glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArrayBuffer);
		glGetIntegerv(GL_ACTIVE_TEXTURE, &previousActiveTexture);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture2d);
		if (!shaderOnly) glGetIntegerv(GL_MATRIX_MODE, &previousMatrixMode);

		glGetIntegerv(GL_VIEWPORT, previousViewport);
		glGetIntegerv(GL_SCISSOR_BOX, previousScissor);
		previousScissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
		previousBlendEnabled = glIsEnabled(GL_BLEND);
		previousDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
		previousStencilEnabled = glIsEnabled(GL_STENCIL_TEST);
		previousCullEnabled = glIsEnabled(GL_CULL_FACE);

		glGetIntegerv(GL_BLEND_SRC_RGB, &previousBlendSrcRgb);
		glGetIntegerv(GL_BLEND_DST_RGB, &previousBlendDstRgb);
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &previousBlendSrcAlpha);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &previousBlendDstAlpha);
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &previousBlendEqRgb);
		glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &previousBlendEqAlpha);

		glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlignment);
		glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &unpackBuffer);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		if (!shaderOnly) {
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
		}

		glActiveTexture(GL_TEXTURE0);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture0);

		for (GLuint i = 0; i < GLuint(attributeCount); ++i) {
			glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &attributeEnabled[i]);
		}
	}

	~SurfaceStateGuard() {
		if (attributeCount < 0) return;
		for (GLuint i = 0; i < GLuint(attributeCount); ++i) {
			if (attributeEnabled[i]) {
				glEnableVertexAttribArray(i);
			} else {
				glDisableVertexAttribArray(i);
			}
		}

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, GLuint(unpackBuffer));
		glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GLuint(texture0));
		if (previousActiveTexture != GL_TEXTURE0) {
			glActiveTexture(GLenum(previousActiveTexture));
			glBindTexture(GL_TEXTURE_2D, GLuint(previousTexture2d));
		}

		if (!shaderOnly) {
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GLenum(previousMatrixMode));
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLuint(previousFramebuffer));
		glBindFramebuffer(GL_READ_FRAMEBUFFER, GLuint(previousReadFramebuffer));
		glBindRenderbuffer(GL_RENDERBUFFER, GLuint(previousRenderbuffer));

		glUseProgram(GLuint(previousProgram));
		glBindBuffer(GL_ARRAY_BUFFER, GLuint(previousArrayBuffer));

		if (previousBlendEnabled) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		glBlendFuncSeparate(previousBlendSrcRgb, previousBlendDstRgb, previousBlendSrcAlpha, previousBlendDstAlpha);
		glBlendEquationSeparate(previousBlendEqRgb, previousBlendEqAlpha);

		if (previousScissorEnabled) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
		if (previousDepthEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		if (previousStencilEnabled) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
		if (previousCullEnabled) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);

		glViewport(previousViewport[0], previousViewport[1], previousViewport[2], previousViewport[3]);
		glScissor(previousScissor[0], previousScissor[1], previousScissor[2], previousScissor[3]);
	}
};

// Batch callbacks use texture unit 0 and generic attributes 0..3. They must not
// mutate matrix stack depth or other texture units. Legacy callbacks retain the
// independently guarded API. Explicit state avoids leaking one pass into another.
void beginShaderPass(bool shaderOnly) {
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
	for (GLuint i = 0; i < 4; ++i) glDisableVertexAttribArray(i);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (!shaderOnly) {
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	}
}
}


struct AdaptiveGlSurface::BatchScope {
	alignas(SurfaceStateGuard) unsigned char storage[sizeof(SurfaceStateGuard)];
	SurfaceStateGuard* guard = nullptr;
	BatchStats* stats;
    bool shaderOnly;
    explicit BatchScope(BatchStats* value, bool restricted):stats(value),shaderOnly(restricted){}
    void enter() { if (!guard) { PhaseTimer timer(stats?&stats->captureNs:nullptr); guard = new (storage) SurfaceStateGuard(4, shaderOnly); } }
    ~BatchScope() { if (guard) { PhaseTimer timer(stats?&stats->restoreNs:nullptr); guard->~SurfaceStateGuard(); } }
};

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
	return renderImpl(targetVg, logicalSize, rackZoom, windowPixelRatio, policy,
		validate, callback, user, nullptr);
}

bool AdaptiveGlSurface::renderBatch(NVGcontext* targetVg, Update* updates, size_t count, BatchStats* stats) {
	if (stats) *stats = {};
	if (!targetVg || (!updates && count) || count > 64) return false;
	if (count && (!APP || !APP->scene || !APP->window || !APP->window->win
		|| (targetVg != APP->window->vg && targetVg != APP->window->fbVg)
		|| glfwGetCurrentContext() != APP->window->win)) return false;
	for (size_t i = 0; i < count; ++i) updates[i].rendered = false;
	for (size_t i = 0; i < count; ++i) {
		const auto& u = updates[i];
		if (!u.surface || !u.callback || !std::isfinite(u.logicalSize.x)
			|| !std::isfinite(u.logicalSize.y) || u.logicalSize.x <= 0 || u.logicalSize.y <= 0
			|| !std::isfinite(u.zoom) || !std::isfinite(u.pixelRatio)
			|| !std::isfinite(u.policy.minDensity) || !std::isfinite(u.policy.maxDensity)
			|| u.policy.maxDensity < .01f || u.policy.sizeQuantum < 1 || u.policy.sizeQuantum > 16384
			|| double(u.logicalSize.x) * u.policy.maxDensity > 16384
			|| double(u.logicalSize.y) * u.policy.maxDensity > 16384
			|| u.policy.vertexAttributeCount < 0 || u.policy.vertexAttributeCount > 4) return false;
		for (size_t j = 0; j < i; ++j) if (updates[j].surface == u.surface) return false;
	}
	bool restricted = count > 0;
	for (size_t i = 0; i < count; ++i) restricted = restricted && updates[i].policy.shaderOnlyState;
	BatchScope scope(stats, restricted);
	for (size_t i = 0; i < count; ++i) {
		auto& u = updates[i];
		u.rendered = u.surface->renderImpl(targetVg, u.logicalSize, u.zoom, u.pixelRatio,
			u.policy, u.validate, u.callback, u.user, &scope);
		if (stats && u.rendered) ++stats->updates;
	}
	if (stats) stats->hostBoundaries = scope.guard ? 1 : 0;
	return true;
}

bool AdaptiveGlSurface::renderImpl(NVGcontext* targetVg, Vec logicalSize,
	float rackZoom, float windowPixelRatio, const AdaptiveGlSurfacePolicy& policy,
	bool validate, RenderCallback callback, void* user, BatchScope* sharedScope) {
	if (!targetVg || !callback || logicalSize.x <= 0.f || logicalSize.y <= 0.f) return false;
	if (vg != targetVg || !gl_lifecycle::resourceContextMatches(resourceContext, targetVg)) {
		reset(false);
		vg = targetVg;
		resourceContext = gl_lifecycle::acquireResourceContext(targetVg);
	}

	float effectiveMaxDensity = policy.maxDensity;
	if (policy.adaptivePressure > 0) {
		const float pressureScale = 1.f / std::sqrt(float(1 + clamp(policy.adaptivePressure, 0, 3)));
		effectiveMaxDensity = std::max(policy.minDensity, policy.maxDensity * pressureScale);
	}
	const float maxDensity = std::max(0.01f, effectiveMaxDensity);
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
	if (sharedScope) sharedScope->enter();
	SurfaceStateGuard stateGuard(sharedScope ? -1 : policy.vertexAttributeCount);
	auto* timing = sharedScope ? sharedScope->stats : nullptr;
    { PhaseTimer timer(timing?&timing->setupNs:nullptr); if (sharedScope) beginShaderPass(sharedScope->shaderOnly); }
    {
    PhaseTimer timer(timing?&timing->targetNs:nullptr);
    if (!ensureBackSurface(targetVg, capacityWidth, capacityHeight, validate)) return false;

	nvgluBindFramebuffer(back);
	// NVGLU marks framebuffer images FLIPY for NanoVG. Rendering against the
	// top edge makes the active prefix addressable with a larger image pattern.
	// Scissor the clear to the active region plus a 1px border so linear filtering
	// at the boundary cannot sample pixels left behind by an earlier, larger render,
	// without clearing the entire peak-capacity texture space every frame.
	const int viewportY = capacityHeight - activeHeight;
	const int clearY = std::max(0, viewportY - 1);
	const int clearWidth = std::min(backCapacityWidth, activeWidth + 1);
	const int clearHeight = backCapacityHeight - clearY;
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, clearY, clearWidth, clearHeight);
	glViewport(0, 0, backCapacityWidth, backCapacityHeight);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
    }
    { PhaseTimer timer(timing?&timing->callbackNs:nullptr);
      callback(user, Vec(float(activeWidth), float(activeHeight)), capacityHeight - activeHeight); }


	std::swap(front, back);
	std::swap(frontCapacityWidth, backCapacityWidth);
	std::swap(frontCapacityHeight, backCapacityHeight);
	frontActiveWidth = activeWidth;
	frontActiveHeight = activeHeight;
	dirty = false;
	++surfaceGeneration;
	return true;
}

bool AdaptiveGlSurface::imageValid() const {
    return vg && gl_lifecycle::resourceContextMatches(resourceContext, vg) && front && front->image>0
        && nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg,front->image,frontCapacityWidth,frontCapacityHeight);
}

bool AdaptiveGlSurface::drawAligned(const Widget::DrawArgs& args, Vec logicalSize, float density, Vec pixelOffset) const {
	if (args.vg != vg || !gl_lifecycle::resourceContextMatches(resourceContext, vg)
		|| !front || front->image <= 0 || !(density > 0.f)) return false;
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, logicalSize.x, logicalSize.y);
	nvgFillPaint(args.vg, nvgImagePattern(args.vg, -pixelOffset.x / density, -pixelOffset.y / density,
		float(frontCapacityWidth) / density, float(frontCapacityHeight) / density, 0.f, front->image, 1.f));
	nvgFill(args.vg);
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
