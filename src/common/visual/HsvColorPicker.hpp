#pragma once

// Reusable HSV picker (saturation/value square + hue strip + before/after
// swatches with hex readout). Shares color math with the Theme editor and is
// designed to sit inside a Rack Menu popover: it reports live edits while
// dragging and a final commit when the drag ends.

#include <rack.hpp>
#include "../../theme/ThemeColorMath.hpp"

#include <cstdio>
#include <functional>

namespace leviathan { namespace visual {

struct HsvColorPicker final : rack::widget::OpaqueWidget {
	enum DragTarget { DragNone, DragSv, DragHue };

	std::function<void(theme::ThemeColor, bool final)> onColorChange;

	theme::ThemeColor color;
	theme::ThemeColor original;
	float hue = 0.f, saturation = 0.f, value = 0.f;
	DragTarget dragTarget = DragNone;

	static constexpr float kPad = 6.f;
	static constexpr float kHueWidth = 14.f;
	static constexpr float kFooter = 22.f;

	HsvColorPicker() { box.size = rack::math::Vec(176.f, 150.f); }

	void setColor(theme::ThemeColor c, bool asOriginal = false) {
		color = c;
		if (asOriginal) original = c;
		float h = hue;
		theme::rgbToHsv(c, &h, &saturation, &value);
		// Keep the previous hue for greys so the strip doesn't snap to red.
		if (saturation > 1e-4f && value > 1e-4f) hue = h;
	}

	rack::math::Rect svRect() const {
		return rack::math::Rect(rack::math::Vec(kPad, kPad),
			rack::math::Vec(box.size.x - 3.f * kPad - kHueWidth, box.size.y - 2.f * kPad - kFooter));
	}
	rack::math::Rect hueRect() const {
		const rack::math::Rect sv = svRect();
		return rack::math::Rect(rack::math::Vec(sv.pos.x + sv.size.x + kPad, sv.pos.y),
			rack::math::Vec(kHueWidth, sv.size.y));
	}
	rack::math::Rect swatchRect(bool originalSwatch) const {
		const rack::math::Rect sv = svRect();
		const float y = sv.pos.y + sv.size.y + kPad;
		const float w = 18.f;
		return rack::math::Rect(rack::math::Vec(originalSwatch ? kPad : kPad + w, y),
			rack::math::Vec(w, kFooter - 4.f));
	}

	void publish(bool final) {
		color = theme::hsvToRgb(hue, saturation, value);
		if (onColorChange) onColorChange(color, final);
	}

	void applyPointer(rack::math::Vec pos) {
		if (dragTarget == DragSv) {
			const rack::math::Rect sv = svRect();
			saturation = rack::math::clamp((pos.x - sv.pos.x) / sv.size.x, 0.f, 1.f);
			value = 1.f - rack::math::clamp((pos.y - sv.pos.y) / sv.size.y, 0.f, 1.f);
		}
		else if (dragTarget == DragHue) {
			const rack::math::Rect hr = hueRect();
			hue = rack::math::clamp((pos.y - hr.pos.y) / hr.size.y, 0.f, 0.9999f);
		}
		publish(false);
	}

	rack::math::Vec localMouse() {
		if (!APP || !APP->scene) return rack::math::Vec();
		return APP->scene->getMousePos().minus(getAbsoluteOffset(rack::math::Vec()))
			.div(std::max(getAbsoluteZoom(), 1e-6f));
	}

	void onButton(const ButtonEvent& e) override {
		if (e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			OpaqueWidget::onButton(e);
			return;
		}
		if (svRect().grow(rack::math::Vec(4.f, 4.f)).contains(e.pos)) dragTarget = DragSv;
		else if (hueRect().grow(rack::math::Vec(4.f, 2.f)).contains(e.pos)) dragTarget = DragHue;
		else if (swatchRect(true).contains(e.pos)) {
			setColor(original);
			publish(true);
			e.consume(this);
			return;
		}
		if (dragTarget != DragNone) {
			applyPointer(e.pos);
			e.consume(this);
			return;
		}
		OpaqueWidget::onButton(e);
	}

	void onDragMove(const DragMoveEvent& e) override {
		if (dragTarget != DragNone) applyPointer(localMouse());
		OpaqueWidget::onDragMove(e);
	}

	void onDragEnd(const DragEndEvent& e) override {
		if (dragTarget != DragNone) {
			dragTarget = DragNone;
			publish(true);
		}
		OpaqueWidget::onDragEnd(e);
	}

	static NVGcolor nvgOf(theme::ThemeColor c) { return nvgRGB(c.r, c.g, c.b); }

	void draw(const DrawArgs& args) override {
		NVGcontext* vg = args.vg;
		const rack::math::Rect sv = svRect();
		nvgBeginPath(vg);
		nvgRect(vg, sv.pos.x, sv.pos.y, sv.size.x, sv.size.y);
		nvgFillColor(vg, nvgOf(theme::hsvToRgb(hue, 1.f, 1.f)));
		nvgFill(vg);
		nvgFillPaint(vg, nvgLinearGradient(vg, sv.pos.x, 0.f, sv.pos.x + sv.size.x, 0.f,
			nvgRGBA(255, 255, 255, 255), nvgRGBA(255, 255, 255, 0)));
		nvgFill(vg);
		nvgFillPaint(vg, nvgLinearGradient(vg, 0.f, sv.pos.y, 0.f, sv.pos.y + sv.size.y,
			nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255)));
		nvgFill(vg);

		const rack::math::Rect hr = hueRect();
		for (int i = 0; i < 6; ++i) {
			const float y0 = hr.pos.y + hr.size.y * i / 6.f;
			const float y1 = hr.pos.y + hr.size.y * (i + 1) / 6.f;
			nvgBeginPath(vg);
			nvgRect(vg, hr.pos.x, y0, hr.size.x, y1 - y0 + 0.5f);
			nvgFillPaint(vg, nvgLinearGradient(vg, 0.f, y0, 0.f, y1,
				nvgOf(theme::hsvToRgb(i / 6.f, 1.f, 1.f)), nvgOf(theme::hsvToRgb((i + 1) / 6.f, 1.f, 1.f))));
			nvgFill(vg);
		}

		nvgBeginPath(vg);
		nvgCircle(vg, sv.pos.x + saturation * sv.size.x, sv.pos.y + (1.f - value) * sv.size.y, 5.f);
		nvgStrokeWidth(vg, 2.f);
		nvgStrokeColor(vg, (value > 0.6f && saturation < 0.4f) ? nvgRGBA(0, 0, 0, 230) : nvgRGBA(255, 255, 255, 245));
		nvgStroke(vg);
		nvgBeginPath(vg);
		nvgRect(vg, hr.pos.x - 2.f, hr.pos.y + hue * hr.size.y - 1.5f, hr.size.x + 4.f, 3.f);
		nvgFillColor(vg, nvgRGBA(255, 255, 255, 245));
		nvgFill(vg);

		// Before/after swatches; clicking "before" reverts.
		for (int i = 0; i < 2; ++i) {
			const rack::math::Rect r = swatchRect(i == 0);
			nvgBeginPath(vg);
			nvgRect(vg, r.pos.x, r.pos.y, r.size.x, r.size.y);
			nvgFillColor(vg, nvgOf(i == 0 ? original : color));
			nvgFill(vg);
			nvgStrokeWidth(vg, 1.f);
			nvgStrokeColor(vg, nvgRGBA(255, 255, 255, 60));
			nvgStroke(vg);
		}
		if (APP && APP->window && APP->window->uiFont) {
			char hex[16];
			std::snprintf(hex, sizeof(hex), "#%02X%02X%02X", color.r, color.g, color.b);
			const rack::math::Rect r = swatchRect(false);
			nvgFontFaceId(vg, APP->window->uiFont->handle);
			nvgFontSize(vg, 11.f);
			nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
			nvgFillColor(vg, nvgRGBA(235, 240, 247, 255));
			nvgText(vg, r.pos.x + r.size.x + 8.f, r.pos.y + r.size.y * 0.5f, hex, nullptr);
		}
		OpaqueWidget::draw(args);
	}
};

}} // namespace leviathan::visual
