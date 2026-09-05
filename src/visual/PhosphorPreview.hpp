#pragma once

#include "../GlLifecycleUtils.hpp"
#include <array>
#include <cmath>

namespace visual_assets {

// GPU-only trail storage. The owning preview supplies history geometry; its
// current curve, labels, and marker remain separate live layers.
template <size_t PointCount>
struct PhosphorPreview : widget::OpenGlWidget {
	bool enabled = false;
	float persistence = 0.6f; // Seconds to approximately 1% remaining brightness.
	float strength = 0.3f;
	bool additive = true;
	std::array<Vec, PointCount> deposit{};
	bool pending = false;
	bool history = false;
	bool resetPending = true;
	double lastCapture = -1.0;
	double lastRender = -1.0;
	double lastDeposit = -1.0;
	GLuint textures[2]{};
	GLuint framebuffers[2]{};
	int width = 0, height = 0, front = 0;
	NVGcontext* owner = nullptr;
	bool failed = false;

	void forgetResources() {
		textures[0] = textures[1] = 0;
		framebuffers[0] = framebuffers[1] = 0;
		width = height = front = 0;
		owner = nullptr;
		history = false;
		resetPending = true;
		lastRender = lastDeposit = -1.0;
		failed = false;
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		forgetResources();
		widget::OpenGlWidget::onContextCreate(e);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		// The old context owns these names. Never issue GL calls at teardown.
		forgetResources();
		widget::OpenGlWidget::onContextDestroy(e);
	}

	void setEnabled(bool value) {
		if (enabled == value) return;
		enabled = value;
		pending = history = false;
		resetPending = true;
		lastCapture = lastRender = lastDeposit = -1.0;
		failed = false;
		setDirty();
	}

	bool capture(const std::array<Vec, PointCount>& points, double now) {
		if (!enabled || (lastCapture >= 0.0 && now - lastCapture < 1.0 / 24.0)) return false;
		deposit = points;
		pending = true;
		lastCapture = now;
		setDirty();
		return true;
	}

	void step() override {
		if (!enabled) {
			widget::Widget::step();
			return;
		}
		if (pending || history || resetPending) setDirty();
		// OpenGlWidget's default step dirties the surface forever. Once the
		// phosphor fades out, retain a clean transparent framebuffer instead.
		widget::FramebufferWidget::step();
	}

	void draw(const DrawArgs& args) override {
		if (enabled) widget::OpenGlWidget::draw(args);
	}

	bool ensureResources(NVGcontext* context, int w, int h) {
		if (!context) return false;
		if (owner != context) {
			forgetResources();
			owner = context;
		}
		if (failed) return false;
		bool valid = gl_lifecycle::isValidTextureFramebufferPair(textures[0], framebuffers[0])
			&& gl_lifecycle::isValidTextureFramebufferPair(textures[1], framebuffers[1]);
		if (valid && w == width && h == height) return true;
		// Delete only validated resources while their owning context is current.
		if (valid) {
			glDeleteFramebuffers(2, framebuffers);
			glDeleteTextures(2, textures);
		}
		textures[0] = textures[1] = framebuffers[0] = framebuffers[1] = 0;
		glGenTextures(2, textures);
		glGenFramebuffers(2, framebuffers);
		width = w; height = h; front = 0;
		history = false;
		resetPending = true;
		for (int i = 0; i < 2; ++i) {
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				glDeleteFramebuffers(2, framebuffers);
				glDeleteTextures(2, textures);
				textures[0] = textures[1] = framebuffers[0] = framebuffers[1] = 0;
				failed = true;
				return false;
			}
			glClear(GL_COLOR_BUFFER_BIT);
		}
		return true;
	}

	void texturedQuad(GLuint texture, float gain) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4f(gain, gain, gain, gain);
		glBegin(GL_QUADS);
		glTexCoord2f(0.f, 1.f); glVertex2f(0.f, 0.f);
		glTexCoord2f(1.f, 1.f); glVertex2f(box.size.x, 0.f);
		glTexCoord2f(1.f, 0.f); glVertex2f(box.size.x, box.size.y);
		glTexCoord2f(0.f, 0.f); glVertex2f(0.f, box.size.y);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	void stampCurve() {
		// Soft-edged ribbons avoid implementation-dependent GL line widths.
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, additive ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);
		const float fringe = box.size.x / float(std::max(width, 1));
		glBegin(GL_QUADS);
		for (size_t i = 1; i < PointCount; ++i) {
			const Vec a = deposit[i - 1], b = deposit[i];
			const Vec delta = b.minus(a);
			const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
			if (length < 1e-5f) continue;
			const Vec normal(-delta.y / length, delta.x / length);
			const float offsets[] = {-0.575f - fringe, -0.575f, 0.575f, 0.575f + fringe};
			const float alpha[] = {0.f, strength, strength, 0.f};
			for (int band = 0; band < 3; ++band) {
				for (int corner = 0; corner < 4; ++corner) {
					const int edge = band + (corner >= 2 ? 1 : 0);
					const Vec p = (corner == 0 || corner == 3 ? a : b).plus(normal.mult(offsets[edge]));
					const float opacity = alpha[edge];
					glColor4f(opacity, opacity * (190.f / 255.f), opacity * (80.f / 255.f), opacity);
					glVertex2f(p.x, p.y);
				}
			}
		}
		glEnd();
		glDisable(GL_BLEND);
	}

	void drawFramebuffer() override {
		renderHistory(APP && APP->window ? APP->window->vg : nullptr,
			getFramebufferSize(), system::getTime());
	}

	// Also usable by a GL smoke harness with its own current context/target.
	void renderHistory(NVGcontext* context, Vec size, double now) {
		GLint target = 0, program = 0, matrixMode = 0, activeTexture = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &target);
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		glMatrixMode(GL_TEXTURE); glPushMatrix(); glLoadIdentity();
		glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
		glOrtho(0.0, box.size.x, box.size.y, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
		glDisable(GL_DEPTH_TEST); glDisable(GL_STENCIL_TEST); glDisable(GL_SCISSOR_TEST);
		glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING); glDisable(GL_ALPHA_TEST);
		glDisable(GL_BLEND); glDisable(GL_DITHER);
		glDisable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glBlendEquation(GL_FUNC_ADD);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glViewport(0, 0, std::max(1, int(size.x)), std::max(1, int(size.y)));
		if (enabled && ensureResources(context, std::max(1, int(size.x)), std::max(1, int(size.y)))) {
			const bool stale = lastDeposit < 0.0 || now - lastDeposit > persistence * 1.5;
			const int back = 1 - front;
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[back]);
			glClear(GL_COLOR_BUFFER_BIT);
			if (!resetPending && history && !stale) {
				// One exponential per rendered frame, never per pixel or audio sample.
				const float dt = float(std::max(0.0, now - lastRender));
				const float fade = std::exp(-4.6051702f * dt / std::max(persistence, 0.01f));
				texturedQuad(textures[front], fade);
			}
			if (resetPending || stale) history = false;
			if (pending && now - lastCapture < 0.1) {
				stampCurve();
				lastDeposit = now;
				history = true;
			}
			pending = resetPending = false;
			lastRender = now;
			front = back;
			glBindFramebuffer(GL_FRAMEBUFFER, GLuint(target));
			glClear(GL_COLOR_BUFFER_BIT);
			if (history) texturedQuad(textures[front], 1.f);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, GLuint(target));
			glClear(GL_COLOR_BUFFER_BIT);
			pending = history = resetPending = false;
		}
		glMatrixMode(GL_MODELVIEW); glPopMatrix();
		glMatrixMode(GL_PROJECTION); glPopMatrix();
		glMatrixMode(GL_TEXTURE); glPopMatrix();
		glPopAttrib();
		glActiveTexture(GLenum(activeTexture));
		glMatrixMode(GLenum(matrixMode));
		glUseProgram(GLuint(program));
		glBindFramebuffer(GL_FRAMEBUFFER, GLuint(target));
	}
};

} // namespace visual_assets
