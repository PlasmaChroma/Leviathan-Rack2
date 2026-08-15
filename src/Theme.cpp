#include "plugin.hpp"

#include "theme/ThemePersistence.hpp"
#include "theme/ThemePresets.hpp"
#include "theme/ThemeService.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {

using leviathan::theme::FactoryPreset;
using leviathan::theme::ThemeColor;
using leviathan::theme::ThemeRole;
using leviathan::theme::ThemeSnapshot;

struct ThemeModule : Module {
	enum ParamId { PARAMS_LEN };
	enum InputId { INPUTS_LEN };
	enum OutputId { OUTPUTS_LEN };
	enum LightId { LIGHTS_LEN };

	ThemeModule() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	}
};

NVGcolor nvgThemeColor(ThemeColor color, unsigned char alpha = 255) {
	return nvgRGBA(color.r, color.g, color.b, alpha);
}

ThemeColor colorForRole(const ThemeSnapshot& snapshot, ThemeRole role) {
	switch (role) {
		case ThemeRole::Input: return snapshot.colors.input;
		case ThemeRole::Output: return snapshot.colors.output;
		case ThemeRole::Accent: return snapshot.colors.accent;
		default: return {};
	}
}

void rgbToHsv(ThemeColor color, float* hue, float* saturation, float* value) {
	const float r = color.r / 255.f;
	const float g = color.g / 255.f;
	const float b = color.b / 255.f;
	const float hi = std::max(r, std::max(g, b));
	const float lo = std::min(r, std::min(g, b));
	const float range = hi - lo;
	float h = 0.f;
	if (range > 1e-6f) {
		if (hi == r) h = std::fmod((g - b) / range, 6.f);
		else if (hi == g) h = (b - r) / range + 2.f;
		else h = (r - g) / range + 4.f;
		h /= 6.f;
		if (h < 0.f) h += 1.f;
	}
	if (hue) *hue = h;
	if (saturation) *saturation = hi > 1e-6f ? range / hi : 0.f;
	if (value) *value = hi;
}

ThemeColor hsvToRgb(float hue, float saturation, float value) {
	hue = hue - std::floor(hue);
	saturation = clamp(saturation, 0.f, 1.f);
	value = clamp(value, 0.f, 1.f);
	const float scaled = hue * 6.f;
	const int sector = int(std::floor(scaled));
	const float fraction = scaled - sector;
	const float p = value * (1.f - saturation);
	const float q = value * (1.f - saturation * fraction);
	const float t = value * (1.f - saturation * (1.f - fraction));
	float r = value, g = t, b = p;
	switch (sector % 6) {
		case 0: r = value; g = t; b = p; break;
		case 1: r = q; g = value; b = p; break;
		case 2: r = p; g = value; b = t; break;
		case 3: r = p; g = q; b = value; break;
		case 4: r = t; g = p; b = value; break;
		case 5: r = value; g = p; b = q; break;
	}
	return ThemeColor(
		std::uint8_t(std::round(r * 255.f)),
		std::uint8_t(std::round(g * 255.f)),
		std::uint8_t(std::round(b * 255.f)));
}

struct ThemeEditor final : TransparentWidget {
	enum DragTarget { DragNone, DragSv, DragHue, DragTexture };

	widget::FramebufferWidget* framebuffer = nullptr;
	ThemeRole selectedRole = ThemeRole::Input;
	DragTarget dragTarget = DragNone;
	std::uint64_t observedGeneration = 0u;
	bool pickerValid = false;
	ThemeRole pickerRole = ThemeRole::None;
	ThemeColor pickerColor;
	float pickerHue = 0.f;
	float pickerSaturation = 0.f;
	float pickerValue = 0.f;
	float retainedHue[3] = {0.f, 0.f, 0.f};

	math::Rect roleRect(int index) const { return math::Rect(Vec(8.f + index * 56.f, 42.f), Vec(52.f, 29.f)); }
	math::Rect svRect() const { return math::Rect(Vec(9.f, 82.f), Vec(137.f, 124.f)); }
	math::Rect hueRect() const { return math::Rect(Vec(151.f, 82.f), Vec(20.f, 124.f)); }
	math::Rect textureRect() const { return math::Rect(Vec(10.f, 239.f), Vec(160.f, 18.f)); }
	math::Rect presetRect(int index) const {
		return math::Rect(Vec(9.f + (index % 2) * 82.f, 274.f + (index / 2) * 29.f), Vec(78.f, 24.f));
	}
	math::Rect swapRect() const { return math::Rect(Vec(9.f, 337.f), Vec(98.f, 27.f)); }
	math::Rect resetRect() const { return math::Rect(Vec(112.f, 337.f), Vec(59.f, 27.f)); }

	Vec currentLocalMousePos() const {
		if (!APP || !APP->scene) return Vec();
		auto* self = const_cast<ThemeEditor*>(this);
		const Vec origin = self->getAbsoluteOffset(Vec());
		const float zoom = std::max(self->getAbsoluteZoom(), 1e-6f);
		return APP->scene->getMousePos().minus(origin).div(zoom);
	}

	void dirty() {
		if (framebuffer) framebuffer->dirty = true;
	}

	int selectedRoleIndex() const {
		switch (selectedRole) {
			case ThemeRole::Input: return 0;
			case ThemeRole::Output: return 1;
			case ThemeRole::Accent: return 2;
			default: return 0;
		}
	}

	void syncPicker(const ThemeSnapshot& snapshot) {
		const ThemeColor current = colorForRole(snapshot, selectedRole);
		if (pickerValid && pickerRole == selectedRole && pickerColor == current) return;
		float hue = 0.f;
		float saturation = 0.f;
		float value = 0.f;
		rgbToHsv(current, &hue, &saturation, &value);
		const int roleIndex = selectedRoleIndex();
		if (saturation > 1e-4f && value > 1e-4f)
			retainedHue[roleIndex] = hue;
		pickerHue = retainedHue[roleIndex];
		pickerSaturation = saturation;
		pickerValue = value;
		pickerColor = current;
		pickerRole = selectedRole;
		pickerValid = true;
	}

	void applyDrag(Vec pos) {
		const ThemeSnapshot snapshot = leviathan::theme::read().snapshot;
		syncPicker(snapshot);
		if (dragTarget == DragSv) {
			const math::Rect area = svRect();
			pickerSaturation = clamp((pos.x - area.pos.x) / area.size.x, 0.f, 1.f);
			pickerValue = 1.f - clamp((pos.y - area.pos.y) / area.size.y, 0.f, 1.f);
			pickerColor = hsvToRgb(pickerHue, pickerSaturation, pickerValue);
			leviathan::theme::setColor(selectedRole, pickerColor);
		}
		else if (dragTarget == DragHue) {
			const math::Rect area = hueRect();
			pickerHue = clamp((pos.y - area.pos.y) / area.size.y, 0.f, 0.999999f);
			retainedHue[selectedRoleIndex()] = pickerHue;
			pickerColor = hsvToRgb(pickerHue, pickerSaturation, pickerValue);
			leviathan::theme::setColor(selectedRole, pickerColor);
		}
		else if (dragTarget == DragTexture) {
			const math::Rect area = textureRect();
			const float normalized = clamp((pos.x - area.pos.x) / area.size.x, 0.f, 1.f);
			leviathan::theme::setTextureAmount(normalized * 2.f);
		}
		dirty();
	}

	void commitDrag() {
		if (dragTarget != DragNone)
			leviathan::theme::persistence::saveToUserStorage();
		dragTarget = DragNone;
	}

	void onButton(const event::Button& e) override {
		if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
			TransparentWidget::onButton(e);
			return;
		}
		if (e.action == GLFW_RELEASE) {
			if (dragTarget != DragNone) {
				commitDrag();
				e.consume(this);
				return;
			}
			TransparentWidget::onButton(e);
			return;
		}
		if (e.action != GLFW_PRESS) {
			TransparentWidget::onButton(e);
			return;
		}

		for (int i = 0; i < 3; ++i) {
			if (roleRect(i).contains(e.pos)) {
				selectedRole = i == 0 ? ThemeRole::Input : (i == 1 ? ThemeRole::Output : ThemeRole::Accent);
				pickerValid = false;
				dirty();
				e.consume(this);
				return;
			}
		}
		if (svRect().contains(e.pos)) dragTarget = DragSv;
		else if (hueRect().contains(e.pos)) dragTarget = DragHue;
		else if (textureRect().contains(e.pos)) dragTarget = DragTexture;
		if (dragTarget != DragNone) {
			applyDrag(e.pos);
			e.consume(this);
			return;
		}

		std::size_t presetCount = 0u;
		const FactoryPreset* presets = leviathan::theme::factoryPresets(&presetCount);
		for (std::size_t i = 0u; i < presetCount && i < 4u; ++i) {
			if (presetRect(int(i)).contains(e.pos)) {
				leviathan::theme::persistence::applyFactoryPresetAndSave(presets[i].id);
				dirty();
				e.consume(this);
				return;
			}
		}
		if (swapRect().contains(e.pos)) {
			ThemeSnapshot snapshot = leviathan::theme::read().snapshot;
			std::swap(snapshot.colors.input, snapshot.colors.output);
			leviathan::theme::apply(snapshot);
			leviathan::theme::persistence::saveToUserStorage();
			dirty();
			e.consume(this);
			return;
		}
		if (resetRect().contains(e.pos)) {
			leviathan::theme::persistence::resetToCanonicalAndSave();
			dirty();
			e.consume(this);
			return;
		}
		TransparentWidget::onButton(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (dragTarget == DragNone || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			TransparentWidget::onDragMove(e);
			return;
		}
		// step() samples the captured pointer once per UI frame. Rack can route
		// drag-move events through the framebuffer inconsistently, and there may
		// be several such events in one frame.
		e.consume(this);
	}

	void onDragEnd(const event::DragEnd& e) override {
		if (dragTarget == DragNone || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			TransparentWidget::onDragEnd(e);
			return;
		}
		commitDrag();
		e.consume(this);
	}

	void step() override {
		if (dragTarget != DragNone) {
			const bool pressed = APP && APP->window && APP->window->win
				&& glfwGetMouseButton(APP->window->win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
			if (pressed)
				applyDrag(currentLocalMousePos());
			else
				commitDrag();
		}
		const std::uint64_t current = leviathan::theme::generation();
		if (current != observedGeneration) {
			observedGeneration = current;
			dirty();
		}
		TransparentWidget::step();
	}

	void text(const DrawArgs& args, float x, float y, float size, const char* value,
		NVGcolor color, int align = NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE) const {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, size);
		nvgTextAlign(args.vg, align);
		nvgFillColor(args.vg, color);
		nvgText(args.vg, x, y, value, nullptr);
	}

	void button(const DrawArgs& args, math::Rect rect, const char* label, bool active = false) const {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, 4.f);
		nvgFillColor(args.vg, active ? nvgRGBA(69, 54, 116, 245) : nvgRGBA(20, 25, 34, 245));
		nvgFill(args.vg);
		nvgStrokeWidth(args.vg, active ? 1.4f : 0.8f);
		nvgStrokeColor(args.vg, active ? nvgRGBA(220, 200, 255, 230) : nvgRGBA(96, 108, 125, 180));
		nvgStroke(args.vg);
		text(args, rect.pos.x + rect.size.x * 0.5f, rect.pos.y + rect.size.y * 0.5f,
			active ? 10.5f : 9.5f, label, nvgRGBA(238, 242, 248, 255));
	}

	void draw(const DrawArgs& args) override {
		const leviathan::theme::ThemeState state = leviathan::theme::read();
		const ThemeSnapshot& snapshot = state.snapshot;
		const ThemeColor selected = colorForRole(snapshot, selectedRole);
		syncPicker(snapshot);
		const float hue = pickerHue;
		const float saturation = pickerSaturation;
		const float value = pickerValue;

		text(args, box.size.x * 0.5f, 17.f, 18.f, "THEME", nvgRGBA(240, 235, 255, 255));
		text(args, box.size.x * 0.5f, 31.f, 8.5f, "GLOBAL CHROMA FIELD", nvgRGBA(129, 148, 166, 255));

		const char* roleNames[] = {"INPUT", "OUTPUT", "ACCENT"};
		const ThemeRole roles[] = {ThemeRole::Input, ThemeRole::Output, ThemeRole::Accent};
		for (int i = 0; i < 3; ++i) {
			button(args, roleRect(i), roleNames[i], selectedRole == roles[i]);
			const ThemeColor swatch = colorForRole(snapshot, roles[i]);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, roleRect(i).pos.x + 5.f, roleRect(i).pos.y + 21.f, 42.f, 3.f, 1.5f);
			nvgFillColor(args.vg, nvgThemeColor(swatch));
			nvgFill(args.vg);
		}

		const math::Rect sv = svRect();
		nvgBeginPath(args.vg);
		nvgRect(args.vg, sv.pos.x, sv.pos.y, sv.size.x, sv.size.y);
		nvgFillColor(args.vg, nvgThemeColor(hsvToRgb(hue, 1.f, 1.f)));
		nvgFill(args.vg);
		NVGpaint white = nvgLinearGradient(args.vg, sv.pos.x, 0.f, sv.pos.x + sv.size.x, 0.f,
			nvgRGBA(255, 255, 255, 255), nvgRGBA(255, 255, 255, 0));
		nvgFillPaint(args.vg, white);
		nvgFill(args.vg);
		NVGpaint black = nvgLinearGradient(args.vg, 0.f, sv.pos.y, 0.f, sv.pos.y + sv.size.y,
			nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255));
		nvgFillPaint(args.vg, black);
		nvgFill(args.vg);

		const math::Rect hr = hueRect();
		for (int i = 0; i < 6; ++i) {
			const float y0 = hr.pos.y + hr.size.y * i / 6.f;
			const float y1 = hr.pos.y + hr.size.y * (i + 1) / 6.f;
			NVGpaint band = nvgLinearGradient(args.vg, 0.f, y0, 0.f, y1,
				nvgThemeColor(hsvToRgb(i / 6.f, 1.f, 1.f)),
				nvgThemeColor(hsvToRgb((i + 1) / 6.f, 1.f, 1.f)));
			nvgBeginPath(args.vg);
			nvgRect(args.vg, hr.pos.x, y0, hr.size.x, y1 - y0 + 0.5f);
			nvgFillPaint(args.vg, band);
			nvgFill(args.vg);
		}

		const float cursorX = sv.pos.x + saturation * sv.size.x;
		const float cursorY = sv.pos.y + (1.f - value) * sv.size.y;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cursorX, cursorY, 5.f);
		nvgStrokeWidth(args.vg, 2.f);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 245));
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, hr.pos.x - 2.f, hr.pos.y + hue * hr.size.y - 1.5f, hr.size.x + 4.f, 3.f);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 245));
		nvgFill(args.vg);

		char hex[16];
		std::snprintf(hex, sizeof(hex), "#%02X%02X%02X", selected.r, selected.g, selected.b);
		text(args, 9.f, 220.f, 11.f, hex, nvgRGBA(235, 240, 247, 255), NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		text(args, 171.f, 220.f, 9.f, "SELECTED ROLE", nvgRGBA(116, 132, 150, 255), NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

		text(args, 9.f, 234.f, 8.5f, "TEXTURE", nvgRGBA(150, 165, 181, 255), NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		const math::Rect texture = textureRect();
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, texture.pos.x, texture.pos.y, texture.size.x, texture.size.y, 4.f);
		nvgFillColor(args.vg, nvgRGBA(19, 24, 32, 255));
		nvgFill(args.vg);
		const float textureWidth = texture.size.x * snapshot.surface.textureAmount * 0.5f;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, texture.pos.x, texture.pos.y, textureWidth, texture.size.y, 4.f);
		nvgFillColor(args.vg, nvgThemeColor(snapshot.colors.accent));
		nvgFill(args.vg);
		char amount[16];
		std::snprintf(amount, sizeof(amount), "%d%%", int(std::round(snapshot.surface.textureAmount * 100.f)));
		text(args, texture.pos.x + texture.size.x * 0.5f, texture.pos.y + texture.size.y * 0.5f,
			9.f, amount, nvgRGBA(245, 247, 251, 255));

		text(args, 9.f, 268.f, 8.5f, "FACTORY FIELDS", nvgRGBA(150, 165, 181, 255), NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		std::size_t presetCount = 0u;
		const FactoryPreset* presets = leviathan::theme::factoryPresets(&presetCount);
		for (std::size_t i = 0u; i < presetCount && i < 4u; ++i)
			button(args, presetRect(int(i)), presets[i].name, state.activePreset == presets[i].id);

		button(args, swapRect(), "SWAP IN / OUT");
		button(args, resetRect(), "RESET", state.activePreset == "factory:leviathan");
		text(args, box.size.x * 0.5f, 373.f, 7.5f, "ONE FIELD  ·  EVERY INSTANCE", nvgRGBA(89, 105, 121, 255));
	}
};

struct ThemeModuleWidget final : ModuleWidget {
	ThemeModuleWidget(ThemeModule* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Theme.svg")));

		auto* framebuffer = new widget::FramebufferWidget;
		framebuffer->box.size = box.size;
		auto* editor = new ThemeEditor;
		editor->box.size = box.size;
		editor->framebuffer = framebuffer;
		framebuffer->addChild(editor);
		addChild(framebuffer);
	}
};

} // namespace

Model* modelTheme = createModel<ThemeModule, ThemeModuleWidget>("Theme");
