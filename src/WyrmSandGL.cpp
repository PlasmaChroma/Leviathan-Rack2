#include "Wyrm.hpp"
#include "WyrmSand.hpp"

#include <chrono>
#include <nanovg_gl.h>

struct WyrmSandGlWidget final : widget::OpenGlWidget {
	Wyrm* module = nullptr;
	std::shared_ptr<WyrmSand> sand;
	GLuint texture = 0;
	int textureW = 0;
	int textureH = 0;
	uint64_t uploadedRevision = 0;

	~WyrmSandGlWidget() override {
		if (texture != 0) {
			glDeleteTextures(1, &texture);
			texture = 0;
		}
	}

	void step() override {
		if (!module) {
			return;
		}
		const int backend = module->sandBackend.load(std::memory_order_relaxed);
		const bool isGlBackend = (backend == WYRMSAND_OPENGL_TEXTURE || backend == WYRMSAND_SHADER_FEEDBACK);
		visible = isGlBackend;
		if (!visible) {
			module->perfSandGlUs.store(0.f, std::memory_order_relaxed);
			return;
		}
		OpenGlWidget::step();
	}

	void drawFramebuffer() override {
		using PerfClock = std::chrono::steady_clock;
		const PerfClock::time_point perfStart = PerfClock::now();
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (!module || !sand) {
			if (module) {
				module->perfSandGlUs.store(0.f, std::memory_order_relaxed);
			}
			return;
		}
		if (!module->sandViewEnabled.load(std::memory_order_relaxed)) {
			module->perfSandGlUs.store(0.f, std::memory_order_relaxed);
			return;
		}

		const int detailSetting = module->sandDetail.load(std::memory_order_relaxed);
		sand->ensureImageRaster(box.size, detailSetting);
		const unsigned char* pixels = sand->imageData();
		const int imageW = sand->imageWidth();
		const int imageH = sand->imageHeight();
		if (!pixels || imageW <= 0 || imageH <= 0) {
			module->perfSandGlUs.store(0.f, std::memory_order_relaxed);
			return;
		}

		if (texture == 0) {
			glGenTextures(1, &texture);
			textureW = 0;
			textureH = 0;
			uploadedRevision = 0;
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		const uint64_t revision = sand->imageDataRevision();
		if (textureW != imageW || textureH != imageH) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageW, imageH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			textureW = imageW;
			textureH = imageH;
			uploadedRevision = revision;
		}
		else if (uploadedRevision != revision) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageW, imageH, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			uploadedRevision = revision;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, box.size.x, box.size.y, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor4f(1.f, 1.f, 1.f, 1.f);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
		glTexCoord2f(1.f, 0.f); glVertex2f(box.size.x, 0.f);
		glTexCoord2f(0.f, 1.f); glVertex2f(0.f, box.size.y);
		glTexCoord2f(1.f, 1.f); glVertex2f(box.size.x, box.size.y);
		glEnd();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glDisable(GL_TEXTURE_2D);
		const float sandGlUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
			PerfClock::now() - perfStart).count()) * 0.001f;
		module->perfSandGlUs.store(sandGlUs, std::memory_order_relaxed);
	}
};

Widget* createWyrmSandGlWidget(Wyrm* module, std::shared_ptr<WyrmSand> sandState) {
	auto* w = new WyrmSandGlWidget();
	w->module = module;
	w->sand = sandState ? sandState : std::make_shared<WyrmSand>();
	return w;
}
